# Bixit

**Bixit** is a binary message dissector that converts raw bits into easy-to-read JSON—and back. It's a highly configurable (de)serialization engine designed to handle even the most demanding use cases, including bit-level precision, complex data structures, and conditional decoding logic.

If your daily work involves reading, interpreting, transforming, injecting or storing binary messages—but building a custom parser isn’t your core business—why waste time reinventing the wheel? Bixit lets you focus on what really matters, while it takes care of the heavy lifting.

## Use Cases

### IoT and Embedded Systems
Parse compact binary protocols from sensors and microcontrollers with bit-level precision and conditional decoding logic.

### Automotive, Aerospace and Railway
Interpret complex data structures from ETSC, CAN, LIN or proprietary binary formats without writing custom parsers.

### Protocol Gateways
Convert binary protocols to JSON for integration with cloud platforms, logging systems, or REST APIs.

## Why Bixit?

Bixit was born out of real-world challenges encountered in the **railway industry**, where a multitude of actors exchange a wide variety of binary messages—each with its own complex format.

In this domain, many systems must interpret these messages to make real-time decisions, log them for auditing, or analyze their patterns for anomalies. Developing software that interacts with such message streams is often **costly and time-consuming**, especially during **prototyping and testing** phases.

During these early stages, having tools that can **decode messages from the device under test**, **inject crafted messages**, and **monitor real-time responses** is key to enabling continuous, hardware-in-the-loop testing. In such environments—where **latency** and **reliability** are critical—it quickly becomes clear that time spent building custom (de)serialization tools is time taken away from focusing on the actual product logic.

Bixit was created to solve exactly this problem: a **fast, deeply configurable**, and **easy-to-integrate dissector** that streamlines the message decoding pipeline, dramatically reducing time and effort, and allowing engineering teams to focus on what matters most—the business logic.

## What is Bixit?

Bixit is a dissector that transforms a generic binary message into its JSON equivalent (and vicevera), based on a configuration file that describes the message format to its internal engine.  
No code to write. No recompilation. No complex logic to manage.

The plain-text configuration file is used in both directions—from binary to JSON and from JSON back to binary.  
Bixit supports multiple configurations simultaneously, which can be neatly organized in a structured catalog of formats using nested directories. This allows seamless management of multiple formats and even different versions of the same format.

Bixit is written in **C++** and provided as a **lightweight, embeddable library**.  
It can be integrated into a wide range of applications: from production software running on the target device, to simulators, testing tools, or even centralized remote decoding services.
