# Analyzer

A tool to easily visualize data from your esp. (it was tested with an esp32 and an arduino uno but other devices should work aswell)
![image info](./docs/image1.PNG)

You can also save an image of every single plot
![image info](./docs/image2.PNG)

# How to use

You have to provide a formatted string via the `print()` function from your esp.  
The string should be in the format of `"plot_name:y_label:graph_name:value\n"`. However all of these values except for `value` can be defaulted thus there's no need to provide them if you don't want to.

Plot name - will be centered above your plot. You can use `##` in front of the name to not show as a title e.g. `##log` - defaults to `"##default`  
Y label - what's shown on the y axis - defaults to `y`  
Graph name - the name of the graph, used to properly assign the values and is shown in the legend - defaults to `f(x)`  
Value - the y axis value, the x axis will always show the elapsed time in seconds - can't be defaulted  
`\n` - The newline is important because it marks the end of an entry thus every entry has to have one

The program sorts the values according to their plot and graph hence make sure to name them properly. You can take a look at the examples provided in the examples/ directory if you want to see it in action.

## Example arduino code

```c++
void setup() 
{
  Serial.begin(9600);
}

static uint32_t i = 0;
void loop() 
{
  Serial.printf("##exp:y:2^n:%.12f\n", pow(2, i)/1000000000000.0);
  Serial.printf("##nlog:y:nlog(n):%.12f\n", i*log(i));
  Serial.printf("##log:y:log(n):%.12f\n", log(i)/100000.0);
  Serial.printf("##log:y:-log(n):%.12f\n", -log(i)/100000.0);
  ++i;
  delay(1000);
}
```

# Build

Windows only! This project uses premake as it's build system. The premake5 binaries are already provided.  
For additional information use:
```
vendor\premake5 --help
```

## Clone

```
git clone https://github.com/pyvyx/Analyzer.git
```
```
cd MouseTracker
```

## Visual Studio

```
vendor\premake5.exe vs2022
```
This should generate a .sln file

## Make

```
vendor\premake5.exe gmake [cc]
```

GCC should already be the default compiler, however you can explicitly specify it if you'd like.  
GCC:   --cc=gcc  
Clang: --cc=clang

### Build

```
make [-j] config=<configuration>
```
Configurations:
 - debug_x86
 - debug_x64 (default, the same as just using `make`)
 - release_x86
 - release_x64

`-j` flag tells make to compile multithreaded

```
make help
```
for additional information