# Coding Conventions

## 1. Scope and Purpose

This document defines the coding conventions to be applied to C source code within this project. The objectives are consistency, readability, and maintainability across all modules and contributors.

These conventions apply to all new code unless explicitly stated otherwise.

## 2. Language and General Rules

* Programming language: **C**.
* All identifiers, keywords, and comments must be written in **English**.
* Code must compile without warnings at the highest reasonable warning level of the selected compiler.

## 3. Naming Conventions

### 3.1 Functions

* Function names must use **lower camel case**.
* Example:

  ```c
  int myFunction(int parameter);
  ```

### 3.2 Variables

* Variable names must use **lower snake case**.
* Names must be descriptive and reflect intent.
* Example:

  ```c
  int my_variable;
  ```

### 3.3 Constants and Macros

* Constants and macros must use **upper snake case**.
* Example:

  ```c
  #define CONSTANTE 42
  ```

### 3.4 File Names

* File names must use lower snake case.
* Header files use the `.h` extension; source files use `.c`.
* Example:

  ```
  module_control.c
  module_control.h
  ```

## 4. Indentation and Formatting

### 4.1 Indentation

* Indentation must use **1 tab**.
* Spaces are strictly forbidden for indentation.

### 4.2 Braces

* Opening braces are placed on the same line as the control statement or function declaration.
* Closing braces are aligned with the corresponding opening statement.

Example:

```c
void myFunction(void) {
  int my_variable = 0;

  if (my_variable > 0) {
    my_variable--;
  }
}
```

### 4.3 Line Length

* Maximum line length: **100 characters**.
* Lines exceeding this limit must be wrapped in a readable manner.

### 4.4 Spacing

* One space after keywords such as `if`, `while`, `for`, and `switch`.
* No extra spaces inside parentheses.
* One space after symbol `;` in `for` loop
* Spaces between expressions in conditions

## 5. Comments

### 5.1 General Rules

* All comments must be written in **English**.
* Comments should explain **why** the code exists or behaves in a certain way, not restate what the code already expresses.

### 5.2 Single-Line Comments

* Use `//` for single-line comments.
* Place comments on their own line, except for very short clarifications.

Example:

```c
// Use an external counter to preserve the result
int counter = 0;
```

### 5.3 Multi-Line Comments

* Use `/* ... */` for multi-line comments.
* Typically used for file headers or detailed explanations.

### 5.4 Function Documentation

* Each public function must be documented using a structured comment block describing purpose, parameters, and return value.

Example:

```c
/*
 * Performs the main processing step of the module.
 *
 * Parameters:
 *   value - input value used for computation
 *
 * Returns:
 *   Result of the computation
 */
int myFunction(int value);
```

## 6. Header Files

* Header files must use include guards.
* Guard names must be uppercase and reflect the file name.

Example:

```c
#ifndef MODULE_CONTROL_H
#define MODULE_CONTROL_H

/* Declarations */

#endif /* MODULE_CONTROL_H */
```

## 7. Error Handling

* Always check the return values of functions that may fail.
* Use explicit error codes or well-defined return values.
* Avoid silent failures.

## 8. Exception: POP Code Reimplementation

When reimplementing or integrating code originating from **POP**, the following exception applies:

* **Original function names must be preserved**.
* **Original variable names must be preserved**.
* No renaming is allowed, even if names do not comply with the conventions defined in this document.

This exception applies strictly and exclusively to POP code reimplementation. All surrounding or newly written code must continue to follow the standard conventions described above.

## 9. Compliance

* All code submissions are expected to comply with this document.
* Deviations must be explicitly justified and documented.
