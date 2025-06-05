package com.google.elemental2.samples.helloworld;

import static elemental2.dom.DomGlobal.document;
import static elemental2.dom.DomGlobal.window;

import jsinterop.annotations.JsType;

@JsType
public class HelloWorld {

  public static void printHelloWorld() {
    window.addEventListener("load", e -> addHellowWorld());
  }

  private static void addHellowWorld() {
    document
        .body
        .appendChild(document.createElement("div"))
        .appendChild(document.createTextNode("Hello from Java!"));
  }
}
