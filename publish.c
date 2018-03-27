#include "service.h"

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <avahi-client/client.h>
#include <avahi-client/publish.h>
#include <avahi-common/simple-watch.h>
#include <avahi-common/malloc.h>
#include <avahi-common/error.h>

#include "utils.h"

#include "_cgo_export.h"

bool create_service(ServiceContext* c);
extern void alternate_service(Service* service);
extern void service_free(Service* service);

void entry_group_callback(AvahiEntryGroup* group, AvahiEntryGroupState state, void* data) {
	ServiceContext* c = (ServiceContext*)data;
	assert(c);
	// Called whenever the entry group state changes
	switch (state) {
	case AVAHI_ENTRY_GROUP_REGISTERING:
	case AVAHI_ENTRY_GROUP_UNCOMMITED:
		// In this cases service shouldn't be freed!
		return;
	case AVAHI_ENTRY_GROUP_COLLISION:
		switch (c->service->collision) {
			case ALTERNATIVE_NAME_ON_COLLISION: 
				avahi_entry_group_reset(group);
				avahi_entry_group_free(group);
				alternate_service(c->service);
				create_service(c);
				return;
			case DO_NOTHING_ON_COLLISION:
			       publishedCallback(c->service, SERVICE_DUPLICATE_NAME_COLLISION, NULL);
			       break;
		}
		break;
	case AVAHI_ENTRY_GROUP_FAILURE :
		publishedCallback(c->service, ENTRY_GROUP_FAILURE, (char*)(avahi_strerror(avahi_client_errno(
						avahi_entry_group_get_client(group)))));
		quit(c->context);
		break;
	case AVAHI_ENTRY_GROUP_ESTABLISHED:
		publishedCallback(c->service, SUCCESSFULL, NULL);
		break;
	}
	service_free(c->service);
	free(c);
}

bool create_service(ServiceContext* c) {
	assert(c);
	int ret = 0;
	AvahiEntryGroup* group = NULL;
	if (!(group = avahi_entry_group_new(c->context->client, entry_group_callback, c))) {
		char* error = concat("avahi_entry_group_new(): ",
				avahi_strerror(avahi_client_errno(c->context->client)));
		clientFailedCallback(ALLOCATION_FAILURE, error);
		free(error);
		service_free(c->service);
		free(c);
		return false;
	}
	while ((ret = avahi_entry_group_add_service(group, AVAHI_IF_UNSPEC, AVAHI_PROTO_UNSPEC, 0, c->service->name,
				c->service->type, c->service->domain, c->service->host, c->service->port, NULL)) < 0) {
		if (ret == AVAHI_ERR_COLLISION)
		{
			switch (c->service->collision) {
				case ALTERNATIVE_NAME_ON_COLLISION: 
					alternate_service(c->service);
					break;
				case DO_NOTHING_ON_COLLISION:
					publishedCallback(c->service, SERVICE_DUPLICATE_NAME_COLLISION, NULL);
					service_free(c->service);
					free(c);
					return true;
			}
		}
		else
		{
			publishedCallback(c->service, SERVICE_REGISTRATION_FAILED, (char*)(avahi_strerror(ret)));
			service_free(c->service);
			free(c);
			return false;
		}
	}
	if ((ret = avahi_entry_group_commit(group)) < 0) {
		publishedCallback(c->service, SERVICE_REGISTRATION_FAILED, (char*)(avahi_strerror(ret)));
		service_free(c->service);
		free(c);
		return false;
	}
	return true;
}

void create_services(Context* c) {
	assert(c);
	if (!c->client_is_running)
		return;
	Service** service = malloc(sizeof(Service**));
	while (publishing(service)) {
		// TODO: Add support of publishing TXT records for services.
		// TODO: Add support of setting different callbacks for registering services.
		// TODO: Add support of removing a resource manually (At termination avahi do this automatically)
		// TODO: Add support of sync publishing
		ServiceContext* service_context = malloc(sizeof(ServiceContext));
		service_context->service = *service;
		service_context->context = c;
		if (!create_service(service_context)) {
			quit(c);
			break;
		}
	}
	free(service);
}
