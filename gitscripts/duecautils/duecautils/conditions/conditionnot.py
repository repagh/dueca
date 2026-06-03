#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Tue Aug 10 17:01:46 2021

@author: repa
"""
import copy
from fractions import Fraction
from .policycondition import ComplexCondition, PolicyCondition, check_and_set
from ..xmlutil import XML_interpret_bool
from ..verboseprint import dprint

def _combine_not(kwargs, inputvar, trim=False):
    """
    Not-calculation of condition test results. If trim=True, only returns
    true values,

    Parameters
    ----------
    kwargs : dict
        Dict with test results.
    inputvar : str
        Name of the variable from the kwargs dict that need to be negated.

    Returns
    -------
    res : list of MatchReference
        Resulting combined variable indicating the "true" matches.

    """
    res = []

    if inputvar not in kwargs:
        raise ValueError(f"Required '{inputvar}' not found in {kwargs.keys()}"
                         " fault in policy XML")

    # this tests the combinations of all inputvars values
    for inputs in kwargs.get(inputvar):

        mr = copy.copy(inputs)

        if (not trim) or mr.value:
            res.append(mr)
            mr.value = not mr.value
    return res

class ConditionNot(ComplexCondition):
    """Negate an incoming condition
    """

    # Determine how param arguments need to be stripped
    default_strip = dict(trim='both', resultvar='both', inputvar='both')

    def __init__(self, trim='false', **kwargs):

        super(ConditionNot, self).__init__(**kwargs)
        if len(self.subconditions) != 1:
            raise ValueError("NOT condition needs 1 subcondition")

        if self.resultvar and (len(self.inputvars) != 1):
            print(self.resultvar, self.inputvars)
            raise ValueError("NOT condition with output needs one inputvar")
        self.trim = XML_interpret_bool(str(trim))

    def holds(self, **kwargs):
        motivation = ['NOT(']
        res, mot, newvars = self.subconditions[0].holds(**kwargs)
        res = Fraction(1) - res

        motivation.extend(mot)
        motivation.append(')')

        if self.resultvar:
            check_and_set(self.resultvar, newvars, _combine_not(
                    newvars, self.inputvars[0], self.trim))

            # update res, if any of the newvars are true, res=true
            res = Fraction(len([r for r in newvars[self.resultvar] if r.value]), max(1, len(newvars[self.resultvar])))

        dprint("ConditionNot", res)
        return (res, motivation, newvars)

PolicyCondition.register("not", ConditionNot)
