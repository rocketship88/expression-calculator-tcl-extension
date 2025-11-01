# Adding Custom Functions

The expression evaluator can be extended with new functions in **3 simple steps**.

## Requirements

- Function names must be 2-5 characters
- Lowercase letters only (a-z)
- Must take one argument

## Step 1: Add to the Enum

In `expression.c`, find the `function_codes` enum and add your function:
```c
enum function_codes {
    eERROR=0, eABS, eINT, eSIN, eCOS, eROUND, eSQRT, eRAD, eFLOOR,
    eLT, eGT, eEQ, eNE, eLE, eGE,
    eMYFUN  // Add here
};
```

## Step 2: Add to the Function Parser

In the `afunc()` function, add your function name:
```c
switch (buf[0]) {
    case 'a':
        if (mystrcmp(buf, "abs") == 0) {
            return eABS;
        }
        break;
    
    case 'm':  // First letter of "myfun"
        if (mystrcmp(buf, "myfun") == 0) {
            return eMYFUNC;
        }
        break;
    
    // ... other cases
}
```

## Step 3: Add to the Evaluation Switch

In `evaluate_d()`, find the comment `// HERE to add additional functions` and add:
```c
switch (f) {
    case eABS:
        if (fval < 0.0) {
            fval = -fval;
        }
        x.val = fval;
        break;
    
    case eMYFUN:
        x.val = /* your calculation here */;
        break;
    
    // ... other cases
}
```

## Example: Adding tan() Function
```c
// Step 1: In enum
eTAN

// Step 2: In afunc()
case 't':
    if (mystrcmp(buf, "tan") == 0) {
        return eTAN;
    }
    break;

// Step 3: In evaluate_d()
case eTAN:
    x.val = tan(fval);
    break;
```

## For Integer-Only Functions (=l)

Repeat the same steps in `evaluate_ll()` instead of `evaluate_d()`.

Note: Only `abs()` and comparison functions (`eq`, `ne`, etc.) are available in `=l`.

## Testing

After making changes:

1. Rebuild: 
2. Test: `= tan(5)`
3. Verify the result

That's it! Three steps and your new function is ready to use.
