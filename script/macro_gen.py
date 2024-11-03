#define __FORMAT_INVOKE_FOREACH_SEP_BY_COLON_1(f, _1) f(_1)
#define __FORMAT_INVOKE_FOREACH_SEP_BY_COLON_2(f, _1, _2) f(_1), f(_2)
with open("output1.cc", "w") as file:
    for i in range(33):
        size = i + 1
        s = f"#define __FORMAT_INVOKE_FOREACH_{size}(f"

        for j in range(size):
            s += f', _{j+1}'
        s += ')'

        for j in range(size):
            s += f' f(_{j+1})'

        file.write(s)
        file.write('\n')
