package main

import (
	"os"
	"os/signal"
	"syscall"
	"github.com/hamed1soleimani/goshneh"
)

func main() {
	c := make(chan os.Signal, 2)
	signal.Notify(c, os.Interrupt, syscall.SIGTERM)
	go func() {
		<-c
		goshneh.Clean()
		os.Exit(0)
	}()
	defer goshneh.Clean()
	goshneh.Setup()
	goshneh.Run()
}
