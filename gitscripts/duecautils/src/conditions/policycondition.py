#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Sun May  2 18:35:52 2021

@author: repa
"""

from ..xmlutil import XML_tag, XML_comment
from ..verboseprint import dprint
from ..param import Param
import sys

class PolicyCondition:

    # dictionary of available conditions
    _conditions = {}

    def __init__(self):
        self.condition = []

    def holds(self, **kwargs):
        raise(Exception("Cannot determine holds, derive "))

    @classmethod
    def register(cls, name, action):
        if name in cls._conditions:
            raise(IndexError(
                    f"Attempting double registration for condition {name}"))
        cls._conditions[name] = action

    @classmethod
    def create(cls, node):

        # condition type
        name = node.get('type')

        # collect the parameters
        params = { }
        for par in node:
            if XML_comment(par):
                continue
            elif XML_tag(par, 'param'):
                p = Param(par,
                    cls._conditions[name].default_strip.get(name, ''))
                params[p.name] = p

        # create the appropriate condition
        return cls._conditions[name](_node=node, **params)


class ComplexCondition(PolicyCondition):

    def __init__(self, _node, resultvar=None, inputvar=None, **kwargs):

        self.subconditions = []
        self.resultvar = None
        try:
            self.resultvar = str(resultvar)
            self.inputvars = list(map(str.strip, str(inputvar).split(',')))
            dprint(f"Compound condition, input {self.inputvars} ({len(self.inputvars)})"
                   f" output {self.resultvar}")
        except AttributeError as e:
            if not(inputvar is None and resultvar is None):
                print(
                    f"Cannot create compound condition, parameter error {e},"
                    f" resultvar:{resultvar} inputvar:{inputvar}"
                    f" kwargs:{kwargs}", file=sys.stderr)
                raise e
            pass

        for sub in _node:
            if XML_comment(sub):
                continue
            elif XML_tag(sub, 'condition'):
                self.subconditions.append(
                        PolicyCondition.create(sub))


class ConditionConstant(PolicyCondition):

    # Determine how param arguments need to be stripped
    default_strip = dict(value='both')

    def __init__(self, **kwargs):
        super(ConditionConstant, self).__init__()
        self.value = bool(kwargs.get('value', False))

    def holds(self, **kwargs):
        return (self.value, [f'Constant condition {self.value}'], dict())

PolicyCondition.register("constant", ConditionConstant)


def checkAndSet(pname, params, value):
    if pname is not None:
        if pname in params:
            print(f"Warning, overwriting parameter {pname}")
        params[pname] = value
