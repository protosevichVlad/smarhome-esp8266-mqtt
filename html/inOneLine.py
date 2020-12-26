import pyperclip

copy = 'String message = "'
with open('sensor/location.html', 'r') as fin:
    for line in fin:
        copy += line.replace('  ', '').replace('\t', '').replace('\n', '\\n').replace('"', '\\"')

copy += '";\n' 
pyperclip.copy(copy)    