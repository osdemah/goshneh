#include "service.h"

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

#include <avahi-client/client.h>
#include <avahi-common/alternative.h>
#include <avahi-common/simple-watch.h>
#include <avahi-common/malloc.h>
#include <avahi-common/error.h>
#include <avahi-common/timeval.h>

void client_callback(AvahiClient* client, AvahiClientState state, void* data);
extern void create_services(Context* c);
extern void browse_for_services(Context* c);

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

void do_waiting_jobs(struct AvahiTimeout* timeout, void* data) {
	Context* c = (Context*)data;
	assert(c);
	create_services(c);
	browse_for_services(c);
	struct timeval tv;
	avahi_simple_poll_get(c->poll)->timeout_update(timeout, avahi_elapse_time(&tv, 100, 0));
}

void client_callback(AvahiClient* client, AvahiClientState state, void* data) {
	Context* c = (Context*)data;
	assert(c);
	c->client = client;
	switch (state) {
		case AVAHI_CLIENT_S_RUNNING:
			c->client_is_running = true;
			struct timeval tv;
			c->registering_interval = avahi_simple_poll_get(c->poll)->timeout_new(avahi_simple_poll_get(
				c->poll), avahi_elapse_time(&tv, 100, 0), do_waiting_jobs, c);
			break;
		case AVAHI_CLIENT_FAILURE:
			c->client_is_running = false;
			fprintf(stderr, "Client failure: %s\n", avahi_strerror(avahi_client_errno(client)));
			quit(c);
			break;
		case AVAHI_CLIENT_CONNECTING:
			c->client_is_running = false;
		case AVAHI_CLIENT_S_COLLISION:
		case AVAHI_CLIENT_S_REGISTERING:
			// TODO: HANDLE REGISTERING and COLLISION states!
			fprintf(stderr, "Client failure due to entering to collision or registering state!\n");
			quit(c);
			break;
			;
	}
}
