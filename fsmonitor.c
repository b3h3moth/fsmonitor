#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <inotifytools/inotifytools.h>
#include <inotifytools/inotify.h>

#define MAX_FILE_NAME 1024

static volatile sig_atomic_t halt_processing = 0;

static void cleanup(int sig) {
   halt_processing = 1;
}

int main(int argc, char *argv[]) {
   int i;
   struct inotify_event *event;
	int events_to_watch = 0;

   // We can accept any number of paths to monitor
   if(argc < 2) {
      printf("Usage: %s <path to monitor> ...\n", argv[0]);
      return -1;
   }

   // Make sure inotify tools initializes properly
   if(!inotifytools_initialize()) {
      fprintf(stderr, "%s\n", strerror(inotifytools_error()));
      return -1;
   }

	// Make sure we get all the useful events
	events_to_watch = IN_CREATE|IN_DELETE|IN_DELETE_SELF|IN_MOVE_SELF|IN_MODIFY;

   // Add listeners for all specified directories
   for(i = 1; i < argc; i++) {
      if(!inotifytools_watch_recursively(argv[i], events_to_watch)) {
         fprintf(stderr, "An error occurred while trying to monitor %s: %s\n", argv[i], strerror(inotifytools_error()));
      }
   }

   // Hijack sigint so we can close cleanly
   signal(SIGINT, cleanup);
   signal(SIGTERM, cleanup);
   signal(SIGQUIT, cleanup);

   // Read all of the events and send any files being written to
   while(!halt_processing && (event = inotifytools_next_event(-1))) {
		inotifytools_printf(event, "%w%f - %e\n");
   }

   printf("\nCleaning up and shutting down\n");
   inotifytools_cleanup();

   return 0;
}

