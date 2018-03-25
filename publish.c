#include "service.h"

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <avahi-client/client.h>
#include <avahi-client/publish.h>
#include <avahi-common/simple-watch.h>
#include <avahi-common/malloc.h>
#include <avahi-common/error.h>

#include "_cgo_export.h"

void entry_group_callback(AvahiEntryGroup* group, AvahiEntryGroupState state, void* data) {
	ServiceContext* c = (ServiceContext*)data;
	// Called whenever the entry group state changes
	switch (state) {
	case AVAHI_ENTRY_GROUP_COLLISION:
		avahi_entry_group_reset(group);
		avahi_entry_group_free(group);
		alternate_service(c->service);
		create_service(c);
		break;
	case AVAHI_ENTRY_GROUP_FAILURE :
		fprintf(stderr, "Entry group failure: %s\n",
			avahi_strerror(avahi_client_errno(avahi_entry_group_get_client(group))));
		quit(c->context);
	case AVAHI_ENTRY_GROUP_ESTABLISHED:
		service_free(c->service);
		free(c);
		break;
	}
}

bool create_service(ServiceContext* c) {
	int ret = 0;
	AvahiEntryGroup* group = NULL;
	if (!(group = avahi_entry_group_new(c->context->client, entry_group_callback, c))) {
		fprintf(stderr, "avahi_entry_group_new() failed: %s\n", avahi_strerror(avahi_client_errno(
						c->context->client)));
		return false;
	}
	while ((ret = avahi_entry_group_add_service(group, AVAHI_IF_UNSPEC, AVAHI_PROTO_UNSPEC, 0, c->service->name,
				c->service->type, c->service->domain, c->service->host, c->service->port, NULL)) < 0) {
		if (ret == AVAHI_ERR_COLLISION)
			alternate_service(c->service);
		else
		{
			fprintf(stderr, "Failed to add %s service of type %s : %s\n", c->service->name,
				c->service->type, avahi_strerror(ret));
			return false;
		}
	}
	if ((ret = avahi_entry_group_commit(group)) < 0) {
		fprintf(stderr, "Failed to commit entry group: %s\n", avahi_strerror(ret));
		return false;
	}
	return true;

}

void create_services(Context* c) {
	assert(c);
	Service** service = malloc(sizeof(Service**));
	int ret = 0;
	while(publishing(service)) {
		ServiceContext* service_context = malloc(sizeof(ServiceContext));
		service_context->service = *service;
		service_context->context = c;
		if (!create_service(service_context))
		{
			free(service);
			return quit(c);
		}
	}
	free(service);
}

void client_callback(AvahiClient* client, AvahiClientState state, void* data) {
	Context* c = (Context*)data;
	assert(c);
	c->client = client;
	switch (state) {
		case AVAHI_CLIENT_S_RUNNING:
			create_services(c);
			break;
		case AVAHI_CLIENT_FAILURE:
			fprintf(stderr, "Client failure: %s\n", avahi_strerror(avahi_client_errno(client)));
			quit(c);
			break;
		case AVAHI_CLIENT_S_COLLISION:
		case AVAHI_CLIENT_S_REGISTERING:
			// TODO: HANDLE REGISTERING and COLLISION states!
			fprintf(stderr, "Client failure due to entering to collision or registering state!\n");
			quit(c);
		case AVAHI_CLIENT_CONNECTING:
			;
	}
}

