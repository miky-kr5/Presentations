from http.server import HTTPServer, SimpleHTTPRequestHandler

def run(server_class = HTTPServer, handler_class = SimpleHTTPRequestHandler):
    server_address = ('127.0.0.1', 8000)
    httpd = server_class(server_address, handler_class)
    httpd.serve_forever()

run()
