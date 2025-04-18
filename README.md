# Bixit
**Bixit** is a configurable binary message dissector. Its goal is to convert the content of a binary message into easy-to-read JSON and back by providing a highly configurable and fast (de)serialization engine designed to handle even the most demanding use cases, including bit-level precision, non-byte-aligned data, complex structures and conditional decoding logic.

## Using Bixit: When and Why
Use Bixit whenever your application needs to interpret structured binary data with precision and flexibility. Designed as a modular message dissector, Bixit can be embedded directly into your software to parse, decode, and transform raw binary messages—such as protocol frames or sensor data—into structured, readable JSON format. It’s especially useful in IoT and embedded systems, where efficient handling of compact binary protocols is critical. In automotive, aerospace and railway domains, Bixit simplifies the interpretation of complex formats like CAN, LIN, or ETCS related. And in gateway scenarios, it bridges binary and web-friendly formats, making integration with cloud services and APIs seamless, without the need for writing custom parsers from scratch.

## Why Bixit?

Bixit was born out of real-world challenges encountered in the **railway industry**, where a multitude of actors exchange a wide variety of binary messages each with its own complex format.

In this domain, many systems must interpret these messages to make real-time decisions, log them for auditing or analyze their patterns for anomalies. Developing software that interacts with such message streams is often **costly and time-consuming**, especially during **prototyping and testing** phases.

During these early stages, having tools that can **decode messages from the device under test**, **inject crafted messages**, and **monitor real-time responses** is key to enabling continuous hardware-in-the-loop testing. In such environments, where **latency** and **reliability** are critical, it quickly becomes clear that time spent building custom (de)serialization tools is time taken away from focusing on the actual product logic.

Bixit was created to solve exactly this problem: a **fast, deeply configurable** and **easy-to-integrate dissector** that streamlines the message decoding pipeline, dramatically reducing time and effort, allowing engineering teams to focus on what matters most: the business logic.

## More details

Bixit is a dissector that transforms a generic binary message into its JSON equivalent (and back), based on a configuration file that describes the message format to its internal engine.  
No code to write. No recompilation. No complex logic to manage.

The plain-text configuration file is used in both directions, from binary to JSON and from JSON back to binary.  
Bixit supports multiple format at the same time, which can be neatly organized in a structured catalog. This allows seamless management of multiple formats and even different versions of the same format.

Bixit is written in **C++** and provided as a **lightweight, linkable library**.  
It can be integrated into a wide range of applications: from production software running on the target device, to simulators, testing tools or even centralized remote decoding services.

Bixit is based on a modular approach, providing an easy way to add new features
