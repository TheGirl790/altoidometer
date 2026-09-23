# pyright: reportUndefinedVariable=false

Import("env")

custom_ld = env.subst("$PROJECT_DIR/linker/memory.ld")

flags = list(env.get("LINKFLAGS", []))

# Remove every occurrence of our custom linker script.
new_flags = []
i = 0

while i < len(flags):
    if flags[i] == "-T" and i + 1 < len(flags):
        if str(flags[i + 1]) == custom_ld:
            i += 2
            continue

    new_flags.append(flags[i])
    i += 1

# Add our override LAST.
new_flags += ["-T", custom_ld]

env.Replace(LINKFLAGS=new_flags)