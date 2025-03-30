Patch Bot -- Autonomous Space Station Repair Robot Human life is priceless. Patch Bot automates space station maintenance to eliminate unnecessary risk. The Problem Currently, astronauts are sent on spacewalks to repair external parts of space stations. While rare, this practice exposes humans to serious risks:

1.  Equipment Failure: A malfunction in the spacesuit---like oxygen loss or temperature issues---can be fatal.
2.  Tether Break: If the tether disconnects, the astronaut could drift away into space.
3.  Space Debris: High-speed space junk (up to 30,000 km/h) can puncture suits or damage tools.

These risks are unacceptable when advanced robotic alternatives are possible. Our Solution Patch Bot is a robotic repair assistant designed to operate outside a space station. It uses gecko-inspired adhesive wheels to move along surfaces and features modular tools like claws, soldering arms, and cargo attachments. The robot performs routine or emergency repairs remotely, reducing the need for risky human EVAs (extravehicular activities). Technology Stack

-   Robot Core: Arduino-based control system with Raspberry Pi communication system
-   Movement: Gecko adhesive wheel system (concept)
-   Communication: Simulated Ultra High Frequency via ESP and Raspberry Pi
-   Control Panel: Built with Next.js for live control from inside the station
-   Data Relay: Raspberry Pi network simulating space-to-Earth communication
-   Backend: Dirigible ERP for storing diagnostic data

Use Case & Audience Patch Bot targets organizations like NASA, SpaceX, and other aerospace agencies. It's ideal for manned missions where preserving crew safety is critical. The robot can handle inspections, patching, and mechanical adjustments---all remotely controlled. Team Yasen Tsvetkov -- Hardware Engineering Borislav Milanov -- Hardware Engineering Valeri Todorov -- Hardware Engineering Simeon Simeonov -- Frontend & Robotics Integration Tomislav Ivanov -- Communication & Backend Systems Emil Momchev -- Pitch & Product Strategy
