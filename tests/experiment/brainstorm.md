## Experiment

- for both tcp and udp communication types
- sample of thirty records

- two clients c0 and c1
- c0 sends message with heavy_operation()
- c1 sends message with heavy_operation()

- record the time differences between iterated select and parallel select
- table with:
    - two columns: one for udp, the other for tcp 
    - two rows: one for threads, the other for iterated serving 

| serving type | TCP | UDP |
|:------------:|:---:|:---:|
|threads       |     |     |
|iterative     |     |     |

- motivation of the experiment is the observation of fast iterative execution

- the values measured are for the second client
- measure how much time it takes to receive the command
- the server is modified such that it for each command there is added a 7-seconds operation 