#ifndef GOSHNEH_SERVICE_H_
#define GOSHNEH_SERVICE_H_

#include <stdlib.h>
#include <stdbool.h>

#include <pthread.h>

#include <avahi-client/client.h>
#include <avahi-client/publish.h>
#include <avahi-common/simple-watch.h>

struct Service
{
	char* name;
	char* type;
	char* domain;
	char* host;
	uint16_t port;
};

struct Context
{
	AvahiSimplePoll* poll;
	AvahiClient* client;
	pthread_mutex_t registering_lock;
	pthread_t registering_thread;
};

typedef struct Service Service;
typedef struct Context Context;

struct ServiceContext
{
	Service* service;
	Context* context;
};

typedef struct ServiceContext ServiceContext;

void client_callback(AvahiClient *client, AvahiClientState state, void* data);

void entry_group_callback(AvahiEntryGroup* group, AvahiEntryGroupState state, void* data);

void* create_services(void* arg);

bool create_service(ServiceContext* c);

void alternate_service(Service* service);

void service_free(Service* service);

void clean(Context* c);

void quit(Context* c);

void setup(Context* c);

void run(Context* c);

#endif
