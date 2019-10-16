#! /usr/bin/env python
import math
import pygame

##############################################################
# Game parameters
##############################################################

TITLE = "Py Caster"
FPS = 60
SCREEN_SIZE = (800, 600)
CEILING_COLOR = (75, 119, 208)
FLOOR_COLOR = (229, 138, 132)
FOG_COLOR = (128, 128, 128)
FOG_NEAR = 1.0
FOG_FAR = 5.0

##############################################################
# Projection parameters
##############################################################

FB_SIZE = (320, 200)
DEG2RAD = 3.1415926535897932384626433 / 180.0
FOV = 66.84962236520761
ANGLE_INCREMENT = FOV / float(FB_SIZE[0])

##############################################################
# Player parameters
##############################################################

PLAYER_TURN_SPEED = 2.0 * DEG2RAD
PLAYER_MOVE_SPEED = 0.1

##############################################################
# Vector Classes
##############################################################

class vec3(object):
    def __init__(self, x = 0, y = 0, z = 0):
        self.x = x
        self.y = y
        self.z = z

    def dot(self, v):
        return (self.x * v.x) + (self.y * v.y) + (self.z * v.z)

    def length(self):
        return math.sqrt(self.dot(self))

    def normalize(self):
        norm = self.length()
        if norm > 0:
            self.x /= norm
            self.y /= norm
            self.z /= norm
        return self

class vec2(object):
    def __init__(self, x = 0, y = 0):
        self.x = x
        self.y = y

    def __str__(self):
        return "(" + str(self.x) + ", " + str(self.y) + ")"

    def add(self, v):
        return vec2(self.x + v.x, self.y + v.y)

    def sub(self, v):
        return vec2(self.x - v.x, self.y - v.y)

    def scale(self, k):
        return vec2(self.x * k, self.y * k)

    def dot(self, v):
        return (self.x * v.x) + (self.y * v.y)

    def length(self):
        return math.sqrt(self.dot(self))

    def distance(self, p):
        return p.sub(self).length()

    def normalize(self):
        norm = self.length()
        if norm > 0:
            self.x /= norm
            self.y /= norm
        return self

    def cross(self, v):
        return vec3(0.0, 0.0, (self.x * v.y) - (self.y * v.x))

##############################################################
# Ray Class
##############################################################

class Ray:
    def __init__(self, origin = vec2(0.0, 0.0), direction = vec2(0.0, 1.0)):
        self.o = origin
        self.d = direction.normalize()


    def __str__(self):
        return "Origin: " + str(self.o) + " :: Direction: " + str(self.d)

##############################################################
# Line Segment Class
##############################################################

class LineSegment:
    def __init__(self, a, b, c = (0, 0, 0)):
        self.a = a
        self.b = b
        self.v = b.sub(a).normalize()
        self.color = c

    def intersect(self, r):
        def sign(n):
            if n == 0:
                return 0
            elif n > 0:
                return 1
            else:
                return -1

        side = self._classifyPoint2D(r.o)

        v2 = self.b.sub(self.a) if sign(side) > 0 else self.a.sub(self.b)
        v3 = vec2(-r.d.y, r.d.x)

        det = v2.dot(v3)

        if abs(det) < 0.00001:
            return None
        else:
            v1 = r.o.sub(self.a) if sign(side) > 0 else self.a.sub(r.o)
            t1 = v2.cross(v1).length() / det
            t2 = v1.dot(v3) / det

            if t2 >= 0.0 and t2 <= 1.0:
                if t1 > 0.0:
                    return r.o.add(r.d.scale(t1))
                else:
                    return None
            else:
                return None

    def _classifyPoint2D(self, point):
        v1 = point.sub(self.a)
        v2 = vec2(self.v.y, -self.v.x)
        return v1.dot(v2)

##############################################################
# Main Function
##############################################################

def main():
    # Local variables.
    done = False
    fog_enabled = False
    toggle_fog = False
    fe_correction = True
    toggle_fe = False
    player_pos = vec2(0.0, 0.0)
    player_dir = vec2(-1.0, 0.0)
    plane = vec2(0.0, 0.66)

    # Initialize Pygame.
    pygame.init()
    clock = pygame.time.Clock()
    screen  = pygame.display.set_mode(SCREEN_SIZE, pygame.HWSURFACE | pygame.DOUBLEBUF)
    frame_buffer = pygame.Surface(FB_SIZE, pygame.HWSURFACE)
    pygame.mouse.set_visible(False)
    pygame.key.set_repeat(17, 17)

    # Define walls.
    walls = [LineSegment(vec2(-3.0, 3.0), vec2(3.0, 3.0), (255, 0, 0)),
             LineSegment(vec2(3.0, 3.0), vec2(3.0, -3.0), (0, 255, 0)),
             LineSegment(vec2(1.5, 1.5), vec2(3.0, 3.0), (255, 255, 0)),
             LineSegment(vec2(3.0, -3.0), vec2(-3.0, -3.0), (0, 0, 255)),
             LineSegment(vec2(-3.0, -3.0), vec2(-3.0, 3.0), (255, 0, 255))]

    # Main game loop.
    try:
        while(not done):
            fps = clock.get_fps() + 0.001
            pygame.display.set_caption(TITLE + ": " + str(int(fps)))
            
            # Input capture.
            for event in pygame.event.get():
                if (event.type == pygame.KEYDOWN and event.key == pygame.K_ESCAPE) or event.type == pygame.QUIT:
                    done = True

                if event.type == pygame.KEYDOWN and event.key == pygame.K_UP:
                    player_pos = player_pos.sub(player_dir.scale(PLAYER_MOVE_SPEED))

                if event.type == pygame.KEYDOWN and event.key == pygame.K_DOWN:
                    player_pos = player_pos.add(player_dir.scale(PLAYER_MOVE_SPEED))

                if event.type == pygame.KEYDOWN and event.key == pygame.K_LEFT:
                    oldDirX = player_dir.x;
                    player_dir.x = player_dir.x * math.cos(PLAYER_TURN_SPEED) - player_dir.y * math.sin(PLAYER_TURN_SPEED);
                    player_dir.y = oldDirX * math.sin(PLAYER_TURN_SPEED) + player_dir.y * math.cos(PLAYER_TURN_SPEED);
                    oldPlaneX = plane.x;
                    plane.x = plane.x * math.cos(PLAYER_TURN_SPEED) - plane.y * math.sin(PLAYER_TURN_SPEED);
                    plane.y = oldPlaneX * math.sin(PLAYER_TURN_SPEED) + plane.y * math.cos(PLAYER_TURN_SPEED);

                if event.type == pygame.KEYDOWN and event.key == pygame.K_RIGHT:
                    oldDirX = player_dir.x;
                    player_dir.x = player_dir.x * math.cos(-PLAYER_TURN_SPEED) - player_dir.y * math.sin(-PLAYER_TURN_SPEED);
                    player_dir.y = oldDirX * math.sin(-PLAYER_TURN_SPEED) + player_dir.y * math.cos(-PLAYER_TURN_SPEED);
                    oldPlaneX = plane.x;
                    plane.x = plane.x * math.cos(-PLAYER_TURN_SPEED) - plane.y * math.sin(-PLAYER_TURN_SPEED);
                    plane.y = oldPlaneX * math.sin(-PLAYER_TURN_SPEED) + plane.y * math.cos(-PLAYER_TURN_SPEED);

                if event.type == pygame.KEYDOWN and event.key == pygame.K_SPACE:
                    if not toggle_fog:
                        fog_enabled = not fog_enabled
                        toggle_fog = True

                if event.type == pygame.KEYUP and event.key == pygame.K_SPACE:
                    if toggle_fog:
                        toggle_fog = False

                if event.type == pygame.KEYDOWN and event.key == pygame.K_f:
                    if not toggle_fe:
                        fe_correction = not fe_correction
                        toggle_fe = True

                if event.type == pygame.KEYUP and event.key == pygame.K_f:
                    if toggle_fe:
                        toggle_fe = False
            
            # Render ceiling and floor.
            frame_buffer.fill(CEILING_COLOR, pygame.Rect(0, 0, FB_SIZE[0], FB_SIZE[1] / 2))
            frame_buffer.fill(FLOOR_COLOR, pygame.Rect(0, FB_SIZE[1] / 2, FB_SIZE[0], FB_SIZE[1] / 2))

            # Render walls.
            angle = -FOV / 2.0
            for i in xrange(FB_SIZE[0]):
                camera_x = 2.0 * (float(i) / float(FB_SIZE[0])) - 1;
                r = Ray(vec2(player_pos.x, player_pos.y), vec2(player_dir.x + plane.x * camera_x, player_dir.y + plane.y * camera_x))

                d = float('Inf')
                c = (0, 0, 0)
                for l in walls:
                    p = l.intersect(r)
                    if p is not None:
                        _d = player_pos.distance(p)
                        if _d < d:
                            d = _d

                            def lerp(col, dst):
                                lt = 0.0 if dst < FOG_NEAR else (1.0 if dst > FOG_FAR else (dst - FOG_NEAR) / (FOG_FAR - FOG_NEAR))

                                red = (FOG_COLOR[0] * lt) + (col[0] * (1.0 - lt))
                                green = (FOG_COLOR[1] * lt) + (col[1] * (1.0 - lt))
                                blue = (FOG_COLOR[2] * lt) + (col[2] * (1.0 - lt))

                                return (red, green, blue)

                            c = lerp(l.color, d) if fog_enabled else l.color

                if d < float('Inf'):
                    h = int(float(FB_SIZE[1]) / (d * math.cos(angle * DEG2RAD))) if fe_correction else int(float(FB_SIZE[1]) / (d))
                    h = FB_SIZE[1] if h > FB_SIZE[1] else h
                    frame_buffer.fill(c, pygame.Rect(i, -(h / 2) + (FB_SIZE[1] / 2), 1, h))

                angle += ANGLE_INCREMENT

            pygame.transform.scale(frame_buffer, SCREEN_SIZE, screen)

            pygame.display.update()
            clock.tick(FPS)
            
    except KeyboardInterrupt:
        pass

    pygame.quit()

if __name__ == "__main__":
    main()

