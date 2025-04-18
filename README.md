# Bixit
**Bixit** is a configurable binary message dissector. Its goal is to convert the content of a binary message into easy-to-read JSON and back by providing a highly configurable and fast (de)serialization engine designed to handle even the most demanding use cases, including bit-level precision, non-byte-aligned data, complex structures and conditional decoding logic.

## Using Bixit: When and Why
Use Bixit whenever your application needs to interpret structured binary data with precision and flexibility. Designed as a modular message dissector, Bixit can be embedded directly into your software to parse, decode, and transform raw binary messages—such as protocol frames or sensor data—into structured, readable JSON format. It’s especially useful in IoT and embedded systems, where efficient handling of compact binary protocols is critical. In automotive, aerospace and railway domains, Bixit simplifies the interpretation of complex formats like CAN, LIN, or ETCS related. And in gateway scenarios, it bridges binary and web-friendly formats, making integration with cloud services and APIs seamless, without the need for writing custom parsers from scratch.

## Genesis of Bixit
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

Bixit is built around a **modular architecture**, making it easy to extend and enhance its capabilities over time. At its core, Bixit acts as a decoding framework that provides all the fundamental components required to interpret structured binary data. These include:
- Format catalog management  
- Dynamic format selection  
- Interfacing hooks  
- Decoding flow control  
- A rich set of base types and logical constructs

### Supported Base Types

- **Unsigned integers**: You can define fields with a custom number of bits (not necessarily multiples of 8) and choose the desired endianness.

### Supported Logical Constructs

- **Data arrays**: Define arrays with either fixed lengths or lengths that depend on the value of other fields in the message.
- **Conditional fields**: Include or exclude parts of the message structure dynamically, based on conditions tied to other field values.
- **Decoding routing**: Direct the decoding flow through specific branches depending on message content.
- **Custom logic**: Extend functionality with user-defined logic via embedded **Lua** scripting.

This flexible internal design makes Bixit a powerful tool for building robust and adaptable message parsers tailored to complex binary protocols.

## Example: CAN format

Here's a simple example of how to define the configuration file used to (de)serialize a CAN message.

### Use Case

Let's say we want to encode/decode CAN (standard) messages consisting of:
- A 1-bit unsigned integer called `start of frame`
- A 11-bit unsigned integer called `identifier`
- Other fields...
- An array of 8-bit unsigned char called `data` (the length of the array depends on the value of a field called `DLC`)
- All the other fields...
  
The final result will be a (de)serialiser able to manage a CAN message like this:
```
000000010100000001000000001000000110100001100000001011111111
```
into a JSON like that:
```json
{
   "start_of_frame": 0,
   "identifier": 20,
   "RTR": 0,
   "IDE": 0,
   "Reserved": 0,
   "DLC": 2,
   "data": [
      1,
      3
   ],
   "CRC": 8576,
   "CRC_delimiter": 1,
   "ACK": 0,
   "ACK_delimiter": 1,
   "EOF": 127
}
```

Here an extraction of the format configuration file (JSON):

```
[...]
"2": {
   "name": "start_of_frame",
   "type": "unsigned integer",
   "next_node": "3",
   "attributes": {
     "bit_length": 1,
     "endianness": "little"
   }
},
"3": {
   "name": "identifier",
   "type": "unsigned integer",
   "next_node": "4",
   "attributes": {
      "bit_length": 11,
       "endianness": "little"
   }
},
[...]
"10": {
   "name": "__array__",
   "type": "array",
   "next_node": "12",
   "attributes": {
      "is_flatten": true,
      "repetitions": "/DLC",
      "array_node_id": "11"
   }
},
"11": {
   "name": "data",
   "type": "unsigned integer",
   "next_node": "",
   "attributes": {
      "bit_length": 8,
      "endianness": "little"
   }
},
[...]
```
## Interface Overview

Bixit exposes a simple yet powerful interface that supports both decoding and encoding operations.

You can:

- **Decode** a message by passing:
  - A binary stream (as a **Base64** string)
  - The **message type identifier** (used to select the correct decoding schema from the catalog)

- **Encode** a message by passing:
  - A **JSON object** with the field values (as a string)
  - The **message type identifier**

In both directions, Bixit automatically selects the correct configuration based on the message type.

Alternatively, for testing or special use cases, you can directly provide the **configuration file** inline with the request—useful for:
- Validating a schema during development
- Encoding or decoding messages using formats that are not part of the current catalog

This flexibility makes Bixit ideal for rapid prototyping, testing and debugging workflows.

