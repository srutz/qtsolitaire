import gdb
import re

class QStringPrinter:
    """Pretty printer for QString"""
    
    def __init__(self, val):
        self.val = val
    
    def to_string(self):
        try:
            d = self.val['d']
            # QString uses UTF-16 encoding
            data_ptr = d.reinterpret_cast(gdb.lookup_type('char').pointer())
            # Skip the QArrayData header (offset field tells us where data starts)
            offset = int(d['offset'])
            size = int(d['size'])
            
            if size == 0:
                return '""'
            
            # Read the UTF-16 data
            data_start = data_ptr + offset
            char_array = []
            for i in range(size):
                # Each QChar is 2 bytes (UTF-16)
                low_byte = int((data_start + i * 2).dereference())
                high_byte = int((data_start + i * 2 + 1).dereference())
                char_code = low_byte | (high_byte << 8)
                
                # Simple ASCII conversion (for basic cases)
                if 32 <= char_code < 127:
                    char_array.append(chr(char_code))
                else:
                    char_array.append('?')
            
            return '"' + ''.join(char_array) + '"'
        except:
            return '<QString>'
    
    def display_hint(self):
        return 'string'

class QByteArrayPrinter:
    """Pretty printer for QByteArray"""
    
    def __init__(self, val):
        self.val = val
    
    def to_string(self):
        try:
            d = self.val['d']
            data_ptr = d.reinterpret_cast(gdb.lookup_type('char').pointer())
            offset = int(d['offset'])
            size = int(d['size'])
            
            if size == 0:
                return '""'
            
            # Read the byte data
            data_start = data_ptr + offset
            byte_array = []
            for i in range(min(size, 200)):  # Limit to 200 chars for safety
                byte_val = int((data_start + i).dereference())
                if 32 <= byte_val < 127:
                    byte_array.append(chr(byte_val))
                else:
                    byte_array.append('?')
            
            result = '"' + ''.join(byte_array) + '"'
            if size > 200:
                result += '...'
            return result
        except:
            return '<QByteArray>'
    
    def display_hint(self):
        return 'string'

def qt_lookup_function(val):
    """Lookup function to determine which pretty printer to use"""
    type_name = str(val.type.unqualified())
    
    if type_name == 'QString':
        return QStringPrinter(val)
    elif type_name == 'QByteArray':
        return QByteArrayPrinter(val)
    
    return None

def register_qt_printers():
    """Register Qt pretty printers"""
    gdb.pretty_printers.append(qt_lookup_function)
    print("Custom Qt pretty printers registered for QString and QByteArray")

# Auto-register when this script is sourced
register_qt_printers()
