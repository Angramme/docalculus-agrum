import re
import sys

def clean(txt):
    txt = txt
    txt = txt.strip()
    txt = re.sub(r"\s+", " ", txt)
    return txt

def trans(txt):
    txt = re.sub(r"//[^\S\n\t]+", "", txt)
    print(txt)
    return (trans_func(txt), trans_comment(txt))

def trans_func(txt):
    fonc = re.compile(r'^\s*def\s+([\w_\d]+)\s*\(((?:\s*[\w_\d]+\s*:?\s*[\w_\d\[\],\"\.]*\s*=?)*)\)\s*->\s*([\w_\d\[\],\"\.]+):\s*$', re.M);
    if not fonc.search(txt): return ("", "", "")

    name, pars, ret = fonc.search(txt).groups()
    pars = pars.split(',')
    params = []
    for p in pars:
        p = p.strip()
        if ":" in p or p == "self":
            params.append(p)
        else:
            params[-1] += ","+p
    pars = []
    for p in params:
        if p == "self": continue
        xx = p.split(":")
        typ = xx[1] if len(xx) > 1 else "?"
        typ = typ.split("=")[0].strip()
        pars.append((xx[0], typ))
    return (name, ret, pars)    

def cpp_fonc(name, ret, pars):
    return f"{ret} {name}({','.join(map(lambda x: f'const {x[1]}& {x[0]}', pars))}) const;"

def trans_comment(txt):
    desc = re.compile(r'"""\n*((?:(?!(?:Paremeters|Returns)\s+-+)(?:.|\n))*)\s*(?:Parameters|Returns)\s+-+', re.M)
    retu = re.compile(r'Returns\s+-+((?:.|\n)*)"""', re.M)
    para = re.compile(r'Parameters\s+-+((?:(?!Returns\s+-+)(?:\n|.))+)\s+(?:Returns\s+-+|""")', re.M)
    paral = re.compile(r'\s*(\w+)\s*:\s*(.*)')

    des = desc.search(txt).group(1) if desc.search(txt) else ""
    ret = retu.search(txt).group(1) if retu.search(txt) else None
    par = para.search(txt).group(1).split('\n') if para.search(txt) else []

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
     {f'* @param {(NEWLINE+"     * @param ").join(map(lambda x: f"{x[0]} {x[1]} : {x[2]}", parameters))}' if parameters else ""}
     {"* @return " + return_desc if return_desc else ""} 
     */
    '''

if __name__ == "__main__":
    s = ""
    l = input()
    while l != "=":
        s += "\n"+l
        l = input()
    tf, tc = trans(s)
    print(tf)
    print(tc)
    print(doxygen(*tc))
    print(cpp_fonc(*tf))