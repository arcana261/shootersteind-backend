# Contributing

When contributing to this repository, please first discuss the change you wish to make via issue,
email, or any other method with the owners of this repository before making a change. 

Please note we have a code of conduct, please follow it in all your interactions with the project.

## Pull Request Process

1. Ensure any install or build dependencies are removed before the end of the layer when doing a 
   build.
2. Update the README.md with details of changes to the interface, this includes new environment 
   variables, exposed ports, useful file locations and container parameters.
3. Increase the version numbers in any examples files and the README.md to the new version that this
   Pull Request would represent. The versioning scheme we use is [SemVer](http://semver.org/).
4. You may merge the Pull Request in once you have the sign-off of two other developers, or if you 
   do not have permission to do that, you may request the second reviewer to merge it for you.

## Code Style

We use Google's code style convention and provide two accompanying tools in order to
lint code against Google's and also to re-format code based on Google's.

### Running Linter

Linter requires `cpplint` to run, so make sure you have `python` and `cpplint` installed.
cpplint can be installed by issueing following command:

```bash
pip install --user cpplint
```

If you have cpplint installed, you can issue linter by running following command:

```bash
make lint
```

### Running Code Formatter

In order to have consistent code formatting across whole project, We have provided
`make format` makefile target which executes `clang-format` tool to re-format code.
In order to use this feature, simply install `clang-format` as follows:

```bash
sudo apt-get install clang-format
```

Additinally, We have provided another target `make check-format` which checks to see
if all files are correctly formatted with `clang-format` tool.

## Generating Documentation

In order to generate doxygen documentation, following requirements must be satisfied:

```bash
sudo apt-get install doxygen graphviz
```

Documents can created by calling correct target for desired format, e.g.
which generates documents in `doc` folder.

```bash
make html
```

## Setting up Visual Studio Code

In case you wish to use visual studio code, you have to first setup includedirs for intellisense
to work.

**First** make sure clang is installed. you can install clang via `apt-get install clang` under ubuntu systems.

**Second**, create a file `.vscode/c_cpp_properties.json` with following example content:

**remember to change `/home/user/` path to appropriate location**

```json
{
    "configurations": [
        {
            "name": "mehdi",
            "includePath": [
                "/usr/include",
                "/usr/include/glib-2.0/glib",
                "/usr/lib/x86_64-linux-gnu/glib-2.0/include",
                "/home/user/uvchan",
                "/home/user/uvchan/src"
            ],
            "intelliSenseMode": "clang-x64",
            "compilerPath": "/usr/bin/clang",
            "cStandard": "c11",
            "cppStandard": "c++17"
        }
    ],
    "version": 4
}
```

## Debugging in Visual Studio Code

Executables forged by autotools toolchain use `libtool` to create a wrapper shell
script with links and runs the programms. This is fine until we want to use GDB debugger.
In that case we need to manually run linked program in `.libs` folder with custom
`LD_LIBRARY_PATH` provided.

**First** Identify which test/tests you wish to debug. As an example, for `xxx_test`
use `test/context/xxx_test -l` to identify available tests, and
`test/context/xxx_test -t <test_path>` to run a specific test.

**Second** Fill `.vscode/launch.json` with appropriate values to be used for debugger.
Listing below shows an example setup in which we try to debug a specific test using
provided `args` configuration.

```json
{
    // Use IntelliSense to learn about possible attributes.
    // Hover to view descriptions of existing attributes.
    // For more information, visit: https://go.microsoft.com/fwlink/?linkid=830387
    "version": "0.2.0",
    "configurations": [
        {
            "name": "xxx_test",
            "type": "cppdbg",
            "request": "launch",
            "program": "${workspaceRoot}/test/uvchan/.libs/xxx_test",
            "args": ["-t", "name_of_the_test"],
            "stopAtEntry": false,
            "cwd": "${workspaceFolder}",
            "environment": [{
                "name": "LD_LIBRARY_PATH",
                "value": "$LD_LIBRARY_PATH:${workspaceRoot}/.libs"
            }],
            "externalConsole": true,
            "MIMode": "gdb",
            "setupCommands": [
                {
                    "description": "Enable pretty-printing for gdb",
                    "text": "-enable-pretty-printing",
                    "ignoreFailures": true
                }
            ]
        }
    ]
}
```

## Code of Conduct

### Our Pledge

In the interest of fostering an open and welcoming environment, we as
contributors and maintainers pledge to making participation in our project and
our community a harassment-free experience for everyone, regardless of age, body
size, disability, ethnicity, gender identity and expression, level of experience,
nationality, personal appearance, race, religion, or sexual identity and
orientation.

### Our Standards

Examples of behavior that contributes to creating a positive environment
include:

* Using welcoming and inclusive language
* Being respectful of differing viewpoints and experiences
* Gracefully accepting constructive criticism
* Focusing on what is best for the community
* Showing empathy towards other community members

Examples of unacceptable behavior by participants include:

* The use of sexualized language or imagery and unwelcome sexual attention or
advances
* Trolling, insulting/derogatory comments, and personal or political attacks
* Public or private harassment
* Publishing others' private information, such as a physical or electronic
  address, without explicit permission
* Other conduct which could reasonably be considered inappropriate in a
  professional setting

### Our Responsibilities

Project maintainers are responsible for clarifying the standards of acceptable
behavior and are expected to take appropriate and fair corrective action in
response to any instances of unacceptable behavior.

Project maintainers have the right and responsibility to remove, edit, or
reject comments, commits, code, wiki edits, issues, and other contributions
that are not aligned to this Code of Conduct, or to ban temporarily or
permanently any contributor for other behaviors that they deem inappropriate,
threatening, offensive, or harmful.

### Scope

This Code of Conduct applies both within project spaces and in public spaces
when an individual is representing the project or its community. Examples of
representing a project or community include using an official project e-mail
address, posting via an official social media account, or acting as an appointed
representative at an online or offline event. Representation of a project may be
further defined and clarified by project maintainers.

### Enforcement

Instances of abusive, harassing, or otherwise unacceptable behavior may be
reported by contacting the project team at [INSERT EMAIL ADDRESS]. All
complaints will be reviewed and investigated and will result in a response that
is deemed necessary and appropriate to the circumstances. The project team is
obligated to maintain confidentiality with regard to the reporter of an incident.
Further details of specific enforcement policies may be posted separately.

Project maintainers who do not follow or enforce the Code of Conduct in good
faith may face temporary or permanent repercussions as determined by other
members of the project's leadership.

### Attribution

This Code of Conduct is adapted from the [Contributor Covenant][homepage], version 1.4,
available at [http://contributor-covenant.org/version/1/4][version]

[homepage]: http://contributor-covenant.org
[version]: http://contributor-covenant.org/version/1/4/
