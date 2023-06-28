import re
import sys

def clean(txt):
    txt = txt
    txt = txt.strip()
    txt = re.sub(r"\s+", " ", txt)
    return txt

def trans(txt):
    txt = re.sub(r"//[^\S\n\t]+", "", txt)
    desc = re.compile(r'"""\n+((?:.|\n)*)\s+Parameters', re.M)
    retu = re.compile(r'Returns\s+-+((?:.|\n)*)"""', re.M)
    para = re.compile(r'Parameters\n\s+-+((?:(?!Returns\s+-+)(?:\n|.))+)\s+(?:Returns\s+-+|""")', re.M)
    paral = re.compile(r'\s+(\w+)\s*:\s*(.*)')

    des = desc.search(txt).group(1)
    ret = retu.search(txt).group(1) if retu.search(txt) else None
    par = para.search(txt).group(1).split('\n')

    pars = []
    for l in par:
        m = paral.match(l)
        if not m: 
            if len(pars) == 0: continue
            pars[-1][2] += "\n"+l
            continue
        pars.append([*m.groups(), ""])
    for p in pars:
        p[2] = clean(p[2]).replace("\n", "\n        ")
    return (clean(des), clean(ret.replace('\n', ' ')) if ret else ret, pars)

def doxygen(description, return_desc, parameters):
    NEWLINE = '\n'
    return f'''
    /**
     * @brief {description.replace(NEWLINE, NEWLINE+"     * ")}
     * 
     * @param {(NEWLINE+"     * @param ").join(map(lambda x: f"{x[0]} {x[1]} : {x[2]}", parameters))}
     {"* @return " + return_desc if return_desc else ""} 
     */
    '''

if __name__ == "__main__":
    s = ""
    l = input()
    while l != "=":
        s += "\n"+l
        l = input()
    print(trans(s))
    print(doxygen(*trans(s)))