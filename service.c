#include "service.h"

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include <avahi-client/client.h>
#include <avahi-common/alternative.h>
#include <avahi-common/simple-watch.h>
#include <avahi-common/malloc.h>
#include <avahi-common/error.h>

void alternate_service(Service* service) {
	char* alternative_name = avahi_alternative_service_name(service->name);
	avahi_free(service->name);
	service->name = alternative_name;
}

void service_free(Service* service) {
	if (service->name)
		free(service->name);
	if (service->type)
		free(service->type);
	if (service->domain)
		free(service->domain);
	if (service->host)
		free(service->host);
	free(service);
}

void clean(Context* c) {
	pthread_mutex_destroy(&(c->registering_lock));
	if (c->client)
		avahi_client_free(c->client);
	if (c->poll)
		avahi_simple_poll_free(c->poll);
}

void quit(Context* c) {
	avahi_simple_poll_quit(c->poll);
	pthread_mutex_trylock(&(c->registering_lock));
	pthread_cancel(c->registering_thread);
	pthread_mutex_unlock(&(c->registering_lock));
	clean(c);
}

void setup(Context* c) {
	int error;

	pthread_mutex_init(&(c->registering_lock), NULL);
	pthread_mutex_lock(&(c->registering_lock));

	// Allocate main loop object
	if (!(c->poll = avahi_simple_poll_new())) {
		fprintf(stderr, "Failed to create simple poll object.\n");
		clean(c);
	}
	// Allocate a new client
	c->client = avahi_client_new(avahi_simple_poll_get(c->poll), 0, client_callback, c, &error);
	// Check wether creating the client object succeeded
	if (!c->client) {
		fprintf(stderr, "Failed to create client: %s\n", avahi_strerror(error));
		clean(c);
	}
}

void run(Context* c) {
	pthread_create(&(c->registering_thread), NULL, create_services, (void*)c);
	avahi_simple_poll_loop(c->poll);
}
