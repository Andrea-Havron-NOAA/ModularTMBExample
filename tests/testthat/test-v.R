library(ModularTMBExample)


v<-CreateVector(100) #new(VariableVector, 100)

#str(v)
#str(v[1])
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


var<-v[1] + cos(v[2])
#str(var)
print(v[1]$value)
print(v[2]$value)
print(cos(v[2]$value))
#str(var)
var$value<-3.1459
#str(var)
#a<-apply(X = v, MARGIN = 1, FUN = sum)
cc<-c(1,2,3)
dim(cc)
l<-v$data
r<-sum(v)
#str(r)
#q()
#str(l)
#a<-lapply(X = l, MARGIN = 1, FUN = sum)
x <- vector("numeric",   length = 10)


typeof(x)



v1<-new(VariableVector, 10)
#str(fill)
#fill(v1,3.1459)

v2<-new(VariableVector, 10)


