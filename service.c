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
	assert(service);
	char* alternative_name = avahi_alternative_service_name(service->name);
	avahi_free(service->name);
	service->name = alternative_name;
}

void service_free(Service* service) {
	assert(service);
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
	assert(c);
	if (c->registering_interval)
		avahi_simple_poll_get(c->poll)->timeout_free(c->registering_interval);
	if (c->client)
		avahi_client_free(c->client);
	if (c->poll)
		avahi_simple_poll_free(c->poll);
}

void quit(Context* c) {
	assert(c);
	avahi_simple_poll_quit(c->poll);
	clean(c);
}

void setup(Context* c) {
	assert(c);
	int error;

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
	assert(c);
	avahi_simple_poll_loop(c->poll);
}
