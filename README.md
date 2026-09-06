# Raven

## About The Project

A personal project for learning how to create a programming language

```txt
main :: (Int, Char^^) -> Int;
main argc argv =
    println("{}", argv[0]); ///< stdout: "Hello world!"
    return 0;
end
```

### Built With

None for now. However, may possibly include dependencies:
- Unicode UTF-8 
- Hash Tables
- Many other optimized implementations...

## Getting Started

To get a local copy up and running, follow these simple steps.

### Prerequisites

- C99 compiler (preferably `gcc` or `clang`)
- ninja
- cmake

### Installation

1. Clone the repo
   ```sh
   git clone https://github.com/eugene-knight/raven.git
   ```
2. Configure and build with cmake
   ```sh
    mkdir build/
    cd build/
    cmake .. -G Ninja
    # If you prefer make
    # cmake ..
   ```
    - 2.1 Optionally run tests
    ```sh
    ctest
    # verbose output: ctest -V
    ```
3. Start the application
   ```sh
    ./raven    
   ```
## Roadmap

- [ ] Minimal **Hello World!** Program
    - [x] Lexer
    - [ ] Parser
    - [ ] C23 Code Generation
- [ ] Robust Error Handling / Reporting
    - [ ] Line / Column
    - [ ] Parser Synchronization
    - [ ] Semantic Analyzer
- [ ] Additional Language Features
    - [ ] User-Defined Types (UDTs)
        - [ ] struct
        - [ ] union
        - [ ] enum

### Roadmap Notes
Each milestone, rigorous tests will be implemented before proceeding to next objective. 

## TL:DR
1. Why **Raven** `rvn`?
    - Because it sounds cool ;) (may change in the future)

## License
Distributed under the GNU General Public License. See `LICENSE.txt` for more information.
