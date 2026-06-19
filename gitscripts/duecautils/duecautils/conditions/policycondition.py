#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Sun May  2 18:35:52 2021

@author: repa

Conditions filter parts from different file types.

- Module condition; filters on module name and donating project name
  * HasModule
- DCO conditions; filters on dco object name and donating project name
  * UsesDCO
  * HomeDCO
- Generic file pattern conditions; may filter either on module, file name regex
  and pattern regex, on platform, file name regex and pattern regex, or only on file
  path regex?
  * FindPattern

For combining filters, a two-stage process is used; first an "any" match is determined
for each filter (like, borrows or has a module, and uses a dco from a specific module)
if true after that, the conditions are run individually, and may produce new variables
to be consumed in a later process. Variables are true if not-empty, and consist of the
combined matches, e.g., comm-objects.lst file, donating project, used dco.
"""

from ..xmlutil import XML_tag, XML_comment
from ..verboseprint import dprint
from ..param import Param
import sys

class PolicyCondition:

    # dictionary of available conditions
    _conditions = {}

    def __init__(self, **kwargs):
        self.condition = []
        unused = [ ka for ka in kwargs.keys() if not (ka.startswith('_') or ka.startswith('p_')) ]
        if unused:
            dprint("Unused parameters:", unused)

    def holds(self, **kwargs):
        """Test this condition, impossible for base PolicyCondition
        """
        raise ValueError("Cannot determine holds, use a derived class, not PolicyCondition")

    @classmethod
    def register(cls, name, action):
        """Register a new condition type

        Parameters
        ----------
        name : str
            XML name for the condition
        action : PolicyCondition
            Condition class
        """
        if name in cls._conditions:
            raise(IndexError(
                    f"Attempting double registration for condition {name}"))
        cls._conditions[name] = action

    @classmethod
    def create(cls, node):
        """Create a condition based on an XML node

        Parameters
        ----------
        node : XML node
            Node with parameters etc for the condition

        Returns
        -------
        PolicyCondition
            One of the condition objects
        """

        # condition type
        name = node.get('type')

        # collect the parameters
        params = { }
        for par in node:
            if XML_comment(par):
                continue
            elif XML_tag(par, 'param'):
                p = Param(par,
                    cls._conditions[name].default_strip.get(
                        par.get('name'), ''))
                params[p.name] = p

        # create the appropriate condition
        return cls._conditions[name](_node=node, **params)


class ComplexCondition(PolicyCondition):

    def __init__(self, _node, resultvar='', inputvar='', **kwargs):
        """Base class for combining / altering conditions

        Parameters
        ----------
        _node : XMLNode
            Current node from which class is made
        resultvar : str, optional
            Name of the result variable, by default ''
        inputvar : str, optional
            Name of detail input variables comma-separated, by default ''
        """

        self.subconditions = []
        self.resultvar = str(resultvar).strip()
        self.inputvars = (str(inputvar) and list(map(str.strip, str(inputvar).split(',')))) or []

        # create the subconditions
        for sub in _node:
            if XML_comment(sub):
                continue
            elif XML_tag(sub, 'condition'):
                self.subconditions.append(
                        PolicyCondition.create(sub))
        super().__init__(**kwargs)

class ConditionConstant(PolicyCondition):
    """ True or false condition

    """
    matchon = set()

    # Determine how param arguments need to be stripped
    default_strip = dict(value='both')

    def __init__(self, **kwargs):
        super(ConditionConstant, self).__init__()
        self.value = bool(kwargs.get('value', False))

    def holds(self, **kwargs):
        return (self.value, [f'Constant condition {self.value}'], dict())

PolicyCondition.register("constant", ConditionConstant)


def check_and_set(pname:str, params:dict, value:list):
    """Helper, to update a parameter workspace with new result values

    Parameters
    ----------
    pname : str|None
        Parameter name
    params : dict
        Current dictionary with parameter
    value : list
        List of result values
    """
    if pname:
        if pname in params:
            print(f"Warning, overwriting parameter {pname}")
        params[str(pname).strip()] = value
        dprint(f"Result: {pname}")
        for r in value:
            dprint(f"  {r}")