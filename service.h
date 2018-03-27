#ifndef GOSHNEH_SERVICE_H_
#define GOSHNEH_SERVICE_H_

#include <stdlib.h>
#include <stdbool.h>

#include <avahi-client/client.h>
#include <avahi-client/publish.h>
#include <avahi-common/simple-watch.h>

enum CollisionBehavior {
	ALTERNATIVE_NAME_ON_COLLISION,
	DO_NOTHING_ON_COLLISION
};

enum Errors {
	SUCCESSFULL,
	SERVICE_DUPLICATE_NAME_COLLISION,
	SERVICE_REGISTRATION_FAILED, 
	CLIENT_FAILURE,
	ENTRY_GROUP_FAILURE,
	ALLOCATION_FAILURE,
	BROWSE_FAILURE,
	RESOLVE_FAILURE
};

enum BrowseEvent {
	SERVICE_RESOLVED = 1,
	SERVICE_REMOVED
};

struct Service {
	char* name;
	char* type;
	char* domain;
	char* host;
	uint16_t port;
	uint8_t collision;
};

struct Context {
	AvahiSimplePoll* poll;
	AvahiClient* client;
	AvahiTimeout* registering_interval;
	bool client_is_running;
};

typedef struct Service Service;
typedef struct Context Context;

struct ServiceContext
{
	Service* service;
	Context* context;
};

typedef struct ServiceContext ServiceContext;

void clean(Context* c);

void quit(Context* c);

void setup(Context* c);

void run(Context* c);

#endif
