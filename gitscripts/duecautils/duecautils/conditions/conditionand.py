#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Tue Aug 10 15:47:33 2021

@author: repa
"""
from fractions import Fraction
import itertools
import copy
from collections import defaultdict
from .policycondition import ComplexCondition, PolicyCondition, check_and_set
from ..xmlutil import XML_interpret_bool
from ..verboseprint import dprint
from ..matchreference import MatchReference


def _combine_iterable(l1, l2):
    return l1 + l2


def _combine_dict(d1, d2):
    res = copy.copy(d1)
    res.update(d2)
    return res


def _combine_first():
    def _combine_first(r1, _r2):
        return r1

    return _combine_first


_funmapping = defaultdict(_combine_first)
_funmapping.update(
    {
        str: _combine_iterable,
        list: _combine_iterable,
        dict: _combine_dict,
        int: _combine_iterable,
    }
)


def _combine_elts(inputvars, selection, ekey, inputs):

    try:
        # combine from multiple?
        eitlist = list(map(str.strip, selection.split(",")))
        # dprint(f"searching {eitlist[0]} in {inputvars}")
        idx = inputvars.index(eitlist[0])
        res = copy.copy(inputs[idx].__dict__[ekey])
        cfun = _funmapping[inputs[idx].__dict__[ekey].__class__]

        for eit in eitlist[1:]:
            idx = inputvars.index(eit)
            res = cfun(res, inputs[idx].__dict__[ekey])
            # res.update(inputs[idx].__dict__[ekey])
        return res
    except Exception as e:
        raise ValueError(f"Cannot transfer/combine property {ekey}, error {e}")


def _combine_and(kwargs, inputvars, matchelts, resultelts, trim):
    """
    And-combination of individual condition test results. If trim=true it
    trims to listing only the true values, if you need to negate the results
    later, use trim=False.

    Parameters
    ----------
    kwargs : dict
        Dict with test results.
    inputvars : list of str
        All variables from the kwargs dict that need to be combined.
    matchelts : list of str
        What should be matched in the combination; consists of the possible
        members in a MatchReference object; typically "module"
        (same module name), "module_project" (project donating the module),
        "dco" (same dco object), "dco_project" (project donating the dco),
        "filename" to match on the given file
    resultelts : dict of str
        Keys in the resultelts dict give the variables in the combined result,
        the associated values indicate which inputvariable from the kwargs
        supplies that value.
    trim : bool
        If true, produce a result with only "true" valued matches, otherwise
        produce all.

    Raises
    ------
    e
        Exception, typically when data members are not correctly specified.

    Returns
    -------
    res : list of MatchReference
        Resulting combined variable indicating the "true" matches.

    """
    res = []
    dprint(f"And combining {inputvars}, on {matchelts}")

    # this tests the combinations of all inputvars values
    for inputs in itertools.product(*map(kwargs.get, inputvars)):

        # inputs is now a tuple of elements from the input variables.
        # check for a match
        matching = True
        value = True
        matchresult = {}
        i0 = inputs[0]

        # check all elements that must match to see if a combination
        # should be made on these inputs
        for elt in matchelts:

            # what is the value on i0?
            eltval = eval(f"i0.{elt}")

            # check further inputs
            for i, iv in enumerate(inputs[1:]):

                nval = eval(f"iv.{elt}")
                if eltval is None:
                    eltval = nval

                elif eltval != nval:
                    # element match value differs
                    matching = False
                    break

            if eltval is None:
                print(f"No value for member {elt}")
            else:
                matchresult[elt] = eltval

        if matching:
            value = i0.value
            for iv in inputs[1:]:
                value = value * iv.value

            if value or (not trim):
                mr = MatchReference(value=value)

                # the matching keys are inserted by default
                for k, v in matchresult.items():
                    mr.__dict__[k] = v
                    dprint(f"setting {k} to {v}")

                # add other results as defined in result-.... values
                for ekey, eit in resultelts.items():
                    mr.__dict__[ekey] = _combine_elts(inputvars, eit, ekey, inputs)
                    dprint(f"Setting {ekey} on new match to {mr.__dict__[ekey]}")

                res.append(mr)

    # dprint(f"result and combination {res}")
    return res


class ConditionAnd(ComplexCondition):
    """And combination of test conditions"""

    # Determine how param arguments need to be stripped
    default_strip = dict(
        trim="both", matchelts="both", resultvar="both", inputvar="both"
    )

    def __init__(self, matchelts=None, trim="false", **kwargs):
        """
        Create an 'and' combination of conditions

        Parameters
        ----------
        matchelts : str, optional
            Comma-separated list of elements to create the 'and' match on.
            Common choices are
            'module': the file matched or condition matched refer to the
                      same module
            'module_project': the module match is from the same parent project
            'dco': match on the same DCO object
            'dco_project': match on the DCO's parent project
            'filename': refers to the same file
            The default is ''.
        **kwargs : dict of str
            parameter arguments for the combination. Parameters interpreted
            by the And combination are:
            'trim': in a result variable, only pass matches with true result
            'result-*': indicate where elements of the result variable come
            from. E.g., an entry with kwargs['result-module'] = 'pattermatch'
            will set the module value from the combined result to the
            module value found in the 'patternmatch' variable produced by
            one of the condition's components.
            The entries 'resultvar' and 'inputvar' will be interpreted by
            the ComplexCondition parent.

        Returns
        -------
        None.

        """
        self.matchelts = (
            matchelts and list(map(str.strip, str(matchelts).split(","))) or []
        )
        self.resultelts = {}
        for key, val in kwargs.items():
            if key.startswith("result_"):
                # dprint(f"result element {key}, value {val}")
                self.resultelts[key[len("result_") :]] = str(val).strip()
        self.trim = XML_interpret_bool(str(trim))
        super(ConditionAnd, self).__init__(**kwargs)

    def holds(self, **kwargs):
        """Test the and condition

        Returns
        -------
        (bool, [MatchReference], dict)
        Result is tuple of
            - A boolean value, True if all of the subordinate conditions
              are True.
            - A "Motivation" list of strings, detailing the reason for the
              return value.
            - A dictionary with resulting variables, if applicable, listing
              the details of the combined check (files with patterns, found
              dco objects or modules, etc.)
            -
        """
        motivation = ["AND("]

        # variables created by subconditions
        newvars = dict()
        _res = Fraction(1)

        for c in self.subconditions:
            res, mot, _nv = c.holds(**kwargs, **newvars)
            _res = _res * res

            newvars.update(_nv)
            motivation.extend(mot)

        if self.resultvar:
            check_and_set(
                self.resultvar,
                newvars,
                _combine_and(
                    newvars, self.inputvars, self.matchelts, self.resultelts, self.trim
                ),
            )
            _res = Fraction(
                len([r for r in newvars[self.resultvar] if r.value]),
                max(1, len(newvars[self.resultvar])),
            )

        motivation.append(")")
        dprint("ConditionAnd", _res)
        return (_res, motivation, newvars)


PolicyCondition.register("and", ConditionAnd)
