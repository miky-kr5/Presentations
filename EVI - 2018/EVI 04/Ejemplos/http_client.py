import urllib2

# Create an HTTP request.
request  = urllib2.Request("http://www.concisa.net.ve/2016/evi-2016-tutoriales/")
# Perform the request.
response = urllib2.urlopen(request)
# Get the response data.
document = response.read()
print(document)
