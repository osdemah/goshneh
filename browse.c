#include "service.h"

#include <avahi-client/client.h>
#include <avahi-client/lookup.h>
#include <avahi-common/simple-watch.h>
#include <avahi-common/malloc.h>
#include <avahi-common/error.h>

#include "_cgo_export.h"

void resolve_callback(AvahiServiceResolver *r, AVAHI_GCC_UNUSED AvahiIfIndex interface,
		AVAHI_GCC_UNUSED AvahiProtocol protocol, AvahiResolverEvent event,
		const char *name, const char *type, const char *domain, const char *host_name,
		const AvahiAddress *address, uint16_t port, AvahiStringList *txt, AvahiLookupResultFlags flags,
		void* data) {
	assert(r);
	Service* service = (Service*)data;
	assert(service);

	service->name = (char*)(name);
	service->type = (char*)(type);
	service->domain = (char*)(domain);
	service->host = (char*)(host_name);
	service->port = port;

	switch (event) {
		case AVAHI_RESOLVER_FAILURE:
			browseCallback(service, 0, RESOLVE_FAILURE, (char*)(avahi_strerror(avahi_client_errno(
							avahi_service_resolver_get_client(r)))));
			break;
		case AVAHI_RESOLVER_FOUND:
			browseCallback(service, SERVICE_RESOLVED, SUCCESSFULL, NULL); 
			break;
	}
	free(service);
	avahi_service_resolver_free(r);
}

void browse_callback(
		AvahiServiceBrowser *b,
		AvahiIfIndex interface,
		AvahiProtocol protocol,
		AvahiBrowserEvent event,
		const char *name,
		const char *type,
		const char *domain,
		AVAHI_GCC_UNUSED AvahiLookupResultFlags flags,
		void* data) {
	Context* c = (Context*)data;
	assert(c);
	assert(b);
	Service* service = (Service*)malloc(sizeof(Service));
	service->name = (char*)(name);
	service->type = (char*)(type);
	service->domain = (char*)(domain);
	service->host = NULL;
	switch (event) {
		case AVAHI_BROWSER_FAILURE:
			browseCallback(service, 0, RESOLVE_FAILURE, NULL); 
			free(service);
			quit(c);
			return;
		case AVAHI_BROWSER_NEW:
			if (!(avahi_service_resolver_new(c->client, interface, protocol, name, type, domain,
							AVAHI_PROTO_UNSPEC, 0, resolve_callback, service)))
				browseCallback(service, 0, RESOLVE_FAILURE, (char*)(avahi_strerror(avahi_client_errno(
									c->client))));
			break;
		case AVAHI_BROWSER_REMOVE:
			browseCallback(service, SERVICE_REMOVED, SUCCESSFULL, NULL); 
			free(service);
			break;
	}
}

void browse_for_services(Context* c) {
	assert(c);
	if (!c->client_is_running)
		return;
	char** type = malloc(sizeof(char**));
	while (browsing(type)) {
		// FIXME: free browser at the end!
		if (!avahi_service_browser_new(c->client, AVAHI_IF_UNSPEC, AVAHI_PROTO_UNSPEC, *type, NULL, 0,
					browse_callback, c)) {
			// TODO: Call failure handle
			// fprintf(stderr, "Failed to create service browser: %s\n", avahi_strerror(avahi_client_errno(client)));
			quit(c);
			break;
		}
	}
	free(type);
}
