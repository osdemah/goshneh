package main

import (
	"fmt"
	"github.com/hamed1soleimani/goshneh"
	"os"
	"os/signal"
	"strconv"
	"syscall"
)

func main() {
	c := make(chan os.Signal, 2)
	signal.Notify(c, os.Interrupt, syscall.SIGTERM)
	go func() {
		<-c
		goshneh.Quit()
		os.Exit(0)
	}()
	defer goshneh.Quit()
	goshneh.Setup()
	goshneh.Run()

	goshneh.PublishedCallback = func(service goshneh.Service, err error) {
		fmt.Println("Published: ", service, err)
	}

	goshneh.ResolvedCallback = func(service goshneh.Service, err error) {
		fmt.Println("Resolved: ", service, err)
	}

	goshneh.RemovedCallback = func(service goshneh.Service) {
		fmt.Println("Removed: ", service)
	}

	if os.Args[1] == "P" {
		goshneh.Publish(goshneh.Service{
			Name:      "TEST",
			Type:      "_http._tcp",
			Port:      80,
			Collision: goshneh.DoNothingOnCollision,
		})

		for i := 0; i < 10; i++ {
			goshneh.Publish(goshneh.Service{
				Name: "TEST" + strconv.Itoa(i/2),
				Type: "_http._tcp",
				Port: 80,
			})
		}
	} else if os.Args[1] == "B" {
		goshneh.Browse("_http._tcp")
	}

	fmt.Scanln()
}
