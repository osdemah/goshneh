package main

import (
	"fmt"
	"github.com/hamed1soleimani/goshneh"
	"os"
	"os/signal"
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
	goshneh.Publish(goshneh.Service{
		Name: "TEST",
		Type: "_http._tcp",
		Port: 80,
	})
	goshneh.Publish(goshneh.Service{
		Name: "TEST1",
		Type: "_http._tcp",
		Port: 80,
	})
	fmt.Scanln()
}
