#include <stdio.h>

#include "unit.h"

int unitWindowOnClosingQuit(uiWindow *w, void *data)
{
	uiQuit();
	return 1;
}

int unitTestsSetup(void **state)
{
	*state = malloc(sizeof(struct state));
	assert_non_null(*state);
	return 0;
}

int unitTestsTeardown(void **state)
{
	free(*state);
	return 0;
}

int unitTestSetup(void **_state)
{
	struct state *state = *_state;
	uiInitOptions o = {0};

	assert_null(uiInit(&o));
	state->w = uiNewWindow("Unit Test", UNIT_TEST_WINDOW_WIDTH, UNIT_TEST_WINDOW_HEIGHT, 0);
	uiWindowOnClosing(state->w, unitWindowOnClosingQuit, NULL);
	return 0;
}

void unitControlOnFree(uiControl *c, void *data)
{
	assert_non_null(c);
	assert_non_null(data);
	assert_ptr_equal(c, ((struct state *)data)->w);
	((struct state *)data)->onFreeCalled = 1;
}

int unitTestTeardown(void **_state)
{
	struct state *state = *_state;

	uiControlOnFree(uiControl(state->w), unitControlOnFree, state);
	state->onFreeCalled = 0;
	uiWindowSetChild(state->w, uiControl(state->c));
	uiControlShow(uiControl(state->w));
	//uiMain();
	uiMainSteps();
	uiMainStep(1);
	uiControlDestroy(uiControl(state->w));
	assert_int_equal(state->onFreeCalled, 1);
	uiUninit();
	return 0;
}

struct unitTest {
	int (*fn)(void);
};

int main(void)
{
	size_t i;
	int failedTests = 0;
	int failedComponents = 0;
	struct unitTest unitTests[] = {
		{ initRunUnitTests },
		{ menuRunUnitTests },
		{ sliderRunUnitTests },
		{ spinboxRunUnitTests },
		{ labelRunUnitTests },
		{ buttonRunUnitTests },
		{ comboboxRunUnitTests },
		{ checkboxRunUnitTests },
		{ radioButtonsRunUnitTests },
		{ entryRunUnitTests },
		{ progressBarRunUnitTests },
	};

	for (i = 0; i < sizeof(unitTests)/sizeof(*unitTests); ++i) {
		int fails = (unitTests[i].fn)();
		failedTests += fails;
		if (fails > 0)
			failedComponents++;
	}

	puts("[==========]");
	if (failedTests == 0)
		puts("[  PASSED  ] All test(s) in all component(s).");
	else
		printf("[  FAILED  ] %d test(s) in %d component(s), see above.\n",
			       failedTests, failedComponents);

	return failedTests;
}

