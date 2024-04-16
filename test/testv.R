library(ModularTMBExample)


v<-new(VariableVector, 100)

str(v)
str(v[1])
v[1]$value
v[1]$value<-1
v[1]$value

for( i in 1:length(v)){
print(v[i]$id)
}

v$resize(5)

for( i in 1:length(v)){
print(v[i]$id)
}

v$resize(10)

for( i in 1:length(v)){
print(v[i]$id)
}
