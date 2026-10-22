interface SubjectParentInterface {
	String PARENT_INTERFACE_NAME = "SubjectParentInterface";

	int sum(int[] values);
}

class SubjectParent implements SubjectParentInterface {
	protected int parentValue;
	protected int[] parentValues;
	protected SubjectParent next;

	public SubjectParent() {
	}

	public SubjectParent(int parentValue) {
		this.parentValue = parentValue;
	}

	public int getParentValue() {
		return parentValue;
	}

	@Override
	public int sum(int[] values) {
		int total = 0;
		for (int value : values) total += value;
		return total;
	}

	public static String message(Throwable throwable) {
		return throwable.getMessage();
	}
}

interface SubjectInterface {
	String join(String[] parts);

	SubjectParent[] parents(int count);
}

public class Subject extends SubjectParent implements SubjectInterface {
	private static int count;
	private double value;
	private String name;

	public Subject() {
	}

	public Subject(double value, String name) {
		this.value = value;
		this.name = name;
	}

	public Subject(SubjectParent parent) {
		this.parentValue = parent.parentValue;
	}

	@Override
	public String join(String[] parts) {
		return String.join(",", parts);
	}

	@Override
	public SubjectParent[] parents(int count) {
		SubjectParent[] parents = new SubjectParent[count];
		for (int i = 0; i < count; i++) parents[i] = new SubjectParent(i);
		return parents;
	}

	public void setValue(double value) {
		this.value = value;
	}

	public double getValue() {
		return value;
	}

	public Subject max(Subject other) {
		return other.value > value ? other : this;
	}

	public String repeat(String text, int count) {
		return text.repeat(count);
	}

	public static long round(double value) {
		return Math.round(value);
	}

	public static int[] range(int count) {
		int[] values = new int[count];
		for (int i = 0; i < count; i++) values[i] = i;
		return values;
	}

	public void fail(String message) {
		throw new IllegalStateException(message);
	}

	public static void failStatic(String message) {
		throw new RuntimeException(message);
	}

	public static Throwable error(String message) {
		return new IllegalArgumentException(message);
	}
}