import urllib.request

# Create an HTTP request and get the response.
response = urllib.request.urlopen("http://www.concisa.net.ve/2016/evi-2016-tutoriales/")
# Get the response data.
document = response.read()
print(document)
