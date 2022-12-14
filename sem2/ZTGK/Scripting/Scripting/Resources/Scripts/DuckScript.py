from API import Vector3
from API import Vector2
from API import Vector4
from API import Bezier
from API import Something
import random
import time
import math

class DuckScript:
    def __init__(self):
        self.gameObject = None
        self.dt = 0.005
        Something.t = 0.0
        self.bezier = Bezier()
        self.lastRand = Vector2(0.0,0.0)
        self.bezier.AddControlPoint(self.lastRand)
        self.NewPoint()
        self.NewPoint()
        self.NewPoint()
        self.pos = self.GetPos()
        self.lastPos=self.pos
        pass

    def RandPoint(self):
        return Vector2(random.random(),random.random())

    def Dist(self,v1,v2):
        x_diff = v1.x-v2.x
        y_diff = v1.y-v2.y
        return math.sqrt(x_diff*x_diff+y_diff*y_diff)
        
    def NewPoint(self):
        point = self.RandPoint()
        while self.Dist(point,self.lastRand)<0.8:
            point = self.RandPoint()
        self.lastRand=point
        self.bezier.AddControlPoint(point)
        pass

    def GetPos(self):
        return Vector3(self.bezier.GetXPos(Something.t), Something.height, self.bezier.GetYPos(Something.t))

    def ConvertPos(self, pos):
        scale = 5;
        translate = -2.5
        return Vector3(scale * pos.x + translate, pos.y, scale*pos.z+ translate)

    def OnCreate(self):
        self.gameObject.SetScale(Vector3(0.01,0.01,0.01))
        pass

    def OnUpdate(self):
        Something.t += self.dt
        if Something.t >= 1.0:
            Something.t = 0.0
            self.NewPoint()

        self.lastPos = self.pos
        self.pos = self.GetPos()
        self.gameObject.SetPosition(self.ConvertPos(self.lastPos))
        self.gameObject.SetNextPosition(self.ConvertPos(self.pos))
        pass