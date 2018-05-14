# goshneh
Golang service discovery library based on avahi

## Usage

### Setup and Start Event Loop

```
goshneh.Setup()
defer goshneh.Quit()
goshneh.Run()
```

### Register Event Handlers

* `PublishedCallback` will be called when introduced service published successfully.
* `ResolvedCallback` will be called when a service in browsing type(s) is found.
* `RemovedCallback` will be called when a service in browsing type(s) is removed.

```
goshneh.PublishedCallback = func(service goshneh.Service, err error) {
        fmt.Println("Published: ", service, err)
}

goshneh.ResolvedCallback = func(service goshneh.Service, err error) {
        fmt.Println("Resolved: ", service, err)
}

goshneh.RemovedCallback = func(service goshneh.Service) {
        fmt.Println("Removed: ", service)
}
```

### Browse for a service type

When any service with given type is resolved or removed related events will be called.

```
 goshneh.Browse("_http._tcp")
```

### Publishing Services

Goshneh will adverties this service and `PublishedCallback` will be called.

```
goshneh.Publish(goshneh.Service{
                Name:      "TEST",
                Type:      "_http._tcp",
                Port:      80,
                Collision: goshneh.DoNothingOnCollision,
})
```

#### Customize collison behaviors

* `AlternativeNameOnCollision` (Default Behavior)

When introducing a service that already introduced with same name goshneh will choose an alternative name for it.

* `DoNothingOnCollision`

Service with duplicated name will be ignored.
