the program determines the status of the server based on the client's request and the server's response
The tree accepts the following file as input(number of: states, input and output symbols, transitions)
transitions look like this (s1 in s2 out)
3 
7
2
10
0 0 1 1
0 6 1 1
1 1 2 1 
1 6 0 0 
1 1 0 0
2 2 2 1
2 3 2 1 
2 4 2 1 
2 5 0 0 
2 6 2 0 

An example of a created homing tree:
Homing tree:
State(s): 0 1 2  (Symbols: 0, 0)
  State(s): 1  (Symbols: 0, 1)
  State(s): 0  (Symbols: 1, 0)
  State(s): 2  (Symbols: 1, 1)
  State(s): 2  (Symbols: 2, 1)
  State(s): 2  (Symbols: 3, 1)
  State(s): 2  (Symbols: 4, 1)
  State(s): 0  (Symbols: 5, 0)
  State(s): 0 2  (Symbols: 6, 0)
    State(s): 1  (Symbols: 0, 1)
    State(s): 2  (Symbols: 2, 1)
    State(s): 2  (Symbols: 3, 1)
    State(s): 2  (Symbols: 4, 1)
    State(s): 0  (Symbols: 5, 0)
    State(s): 2  (Symbols: 6, 0)
    State(s): 1  (Symbols: 6, 1)
  State(s): 1  (Symbols: 6, 1)

  
