package miky;
import robocode.*;
import java.util.List;
import java.util.LinkedList;

/**
 * SubsumpRobot - a robot by (your name here)
 */
public class SubsumpRobot extends Robot {
	abstract class SubsumptionBehavior {
		public abstract boolean takeControl();
		public abstract void action();
	}
	
	class WanderBehavior extends SubsumptionBehavior {
		public boolean takeControl() {
			return true;
		}
	
		public void action() {
			ahead(200);
			if (Math.random() > 0.5)
				turnLeft(30.0);
			else
				turnRight(30.0);
			scan();
		}
	}

	class MoveToRobotBehavior extends SubsumptionBehavior {
		public boolean takeControl() {
			return scannedRobot != null;
		}
	
		public void action() {
			turnRight(scannedRobot.getBearing());
			ahead(50);
		}
	}

	class FireBehavior extends SubsumptionBehavior {
		public boolean takeControl() {
			return scannedRobot != null && scannedRobot.getDistance() < 200;
		}
	
		public void action() {
			fire(3);
		}
	}
	
	class AvoidWallBehavior extends SubsumptionBehavior {
		public boolean takeControl() {
				return hitEvent != null;
		}
	
		public void action() {
			hitEvent = null;
			scannedRobot = null;
			back(100);
			turnLeft(180.0);
		}
	}
	
	private List<SubsumptionBehavior> behaviors;
	private ScannedRobotEvent scannedRobot;
	private Event hitEvent;
		
	public SubsumpRobot() {
		super();
		hitEvent = null;
		scannedRobot = null;
		behaviors = new LinkedList<SubsumptionBehavior>();
	}
	
	public void addBehavior(SubsumptionBehavior behavior) {
		behaviors.add(behavior);
	}
	
	public void removeBehavior(SubsumptionBehavior behavior) {
		behaviors.remove(behavior);	
	}
	
	/**
	 * run: SubsumpRobot's default behavior
	 */
	public void run() {
		int behavior_index = 0;

		// Add behaviors.
		behaviors.add(new AvoidWallBehavior());
		behaviors.add(new FireBehavior());
		behaviors.add(new MoveToRobotBehavior());
		behaviors.add(new WanderBehavior());

		while(true) {
			for (SubsumptionBehavior b : behaviors) {
				if (b.takeControl()) {
					b.action();
					break;
				}
			}
		}
	}

	public void onScannedRobot(ScannedRobotEvent e) {	
		scannedRobot = e;
	}

	public void onHitWall(HitWallEvent e) {
		hitEvent = e;	
	}

	public void onHitRobot(HitRobotEvent e) {
		hitEvent = e;	
	}

	public void onRobotDeath(RobotDeathEvent e) {
		scannedRobot = null;
	}
}
