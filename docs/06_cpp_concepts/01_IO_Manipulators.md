# C++ I/O Manipulators

To create perfectly aligned tables in C++, we use "I/O manipulators" from the `<iomanip>` library. They allow us to control exactly how many characters an output takes up and how it is aligned.

Here is a breakdown of the specific manipulators commonly used to create a tabular format:

## 1. `std::setw(n)` (Set Width)
This tells the console: *"Reserve exactly `n` character spaces for the next thing I print."* 
If the text is shorter than `n` characters, it fills the remaining space with empty spaces (padding). 
* **Important:** Unlike other manipulators, `std::setw` only applies to the **very next** item you print. You have to call it again for every single column.

## 2. `std::left` (Left Alignment)
By default, if you reserve 10 spaces for a 4-letter word using `setw(10)`, C++ will push the word to the right and put the 6 empty spaces on the left (e.g., `"      Word"`). 
`std::left` changes this behavior so the text sticks to the left and spaces are added to the right (e.g., `"Word      "`). 
* **Important:** This setting is "sticky." Once you use `std::left`, everything stays left-aligned until you explicitly change it back (e.g., with `std::right`).

## 3. `std::fixed` and `std::setprecision(n)`
These are used together to format floating-point numbers:
* **`std::fixed`**: Forces numbers to be printed as normal decimals instead of scientific notation (e.g., it prevents `0.001` from printing as `1e-3`).
* **`std::setprecision(n)`**: When used with `std::fixed`, this forces the console to always print exactly `n` digits after the decimal point, even if the number is perfectly whole (e.g., `5.000`). This ensures decimal points line up vertically in the table.

---

## How they work together (Visual Example)

Looking at this line from our benchmarking code:
```cpp
std::cout << std::left << std::setw(10) << gridStr << " | "
          << std::setw(10) << dijTime << "\n";
```

1. `std::left` tells C++ to align all future padded text to the left.
2. `std::setw(10) << gridStr` reserves 10 spaces. If `gridStr` is `"10x10"`, it prints the 5 characters and pads it with 5 spaces: `"10x10     "`.
3. `<< " | "` prints a literal separator border.
4. `std::setw(10) << dijTime` reserves 10 spaces for the time variable.

**Resulting Output:**
```text
10x10      | 0.125     
32x32      | 1.450     
100x100    | 12.500    
```
By reserving exactly the same number of character spaces for a column on every iteration of a loop, everything forms a perfectly straight table regardless of how long the actual text is!
