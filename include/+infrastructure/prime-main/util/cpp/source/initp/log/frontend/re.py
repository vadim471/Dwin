import re

def main():
    with open('dispenser_request.cpp', 'r') as f:
        content = f.read()
        content = re.sub(r"log\:\:(fatal|error|warning)\((.*?), (.*?), (.*)\)\;", r"log::\1(\3, \2) << \4;", content)
        content = re.sub(r"log\:\:(info|debug|trace)\((.*?), (.*)\)\;", r"log::\1(\2) << \3;", content)
    with open('dispenser_request.cpp', 'w') as f:
        f.write(content)

if __name__ == '__main__':
    main()
