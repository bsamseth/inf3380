### Code Structure

Process 0 is used as a master, with the remaining processes (at least 2) called workers. The master
only delegates work to the workers, and collects the results in the end.
This seperation is done in order to get more readable code (IMO).

### Credit
Code design is inspired by looking at [this](https://github.com/jvbrink/coursework/tree/master/INF3380/oblig1) repository,
as well as the guidelines in the [exercise text](../exercise_text.pdf).
