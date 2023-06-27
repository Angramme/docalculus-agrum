import re
import sys

def clean(txt):
    txt = txt
    txt = txt.strip()
    txt = re.sub(r"\s+", " ", txt)
    return txt

def trans(txt):
    desc = re.compile(r'"""\n+((?:.|\n)*)\s+Parameters', re.M)
    retu = re.compile(r'Returns\s+-+((?:.|\n)*)"""', re.M)
    para = re.compile(r'Parameters\n\s+-+((?:\n|.)+)\s+Returns')
    paral = re.compile(r'\s+(\w+)\s*:\s*(.*)')

    des = desc.search(txt).group(1)
    ret = retu.search(txt).group(1)
    par = para.search(txt).group(1).split('\n')

    pars = []
    for l in par:
        m = paral.match(l)
        if not m: continue
        pars.append(m.groups())
    return (clean(des), clean(ret.replace('\n', ' ')), pars)

def doxygen(description, return_desc, parameters):
    NEWLINE = '\n'
    return f'''
    /**
     * @brief {description.replace(NEWLINE, NEWLINE+"     * ")}
     * 
     * @param {(NEWLINE+"     * @param ").join(map(lambda x: f"{x[0]} {x[1]}", parameters))}
     * @return {return_desc} 
     */
    '''

if __name__ == "__main__":
    s = ""
    l = input()
    while l != "=":
        s += "\n"+l
        l = input()
    print(doxygen(*trans(s)))