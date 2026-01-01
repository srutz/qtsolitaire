set print pretty on
set print object on
set print static-members on
set print vtbl on
set print demangle on
set demangle-style gnu-v3
set print sevenbit-strings off

python
import sys
import os

# Add current directory to path for custom pretty printers
current_dir = os.path.dirname(os.path.abspath(__file__)) if '__file__' in dir() else os.getcwd()
if current_dir not in sys.path:
    sys.path.insert(0, current_dir)

# Try to load custom Qt pretty printers
try:
    gdbinit_py = os.path.join(current_dir, '.gdbinit.py')
    if os.path.exists(gdbinit_py):
        with open(gdbinit_py, 'r') as f:
            exec(f.read())
    else:
        print("Warning: .gdbinit.py not found")
except Exception as e:
    print(f"Could not load custom Qt printers: {e}")

end

printf "Custom .gdbinit loaded\n"

