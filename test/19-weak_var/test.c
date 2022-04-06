extern int __attribute__((weak)) weak_var;

int function()
{ return weak_var; }
