#!/usr/bin/env python3
# -*- coding: utf-8 -*-
"""
Created on Wed Jun 30 20:56:53 2021

@author: repa
"""
from fractions import Fraction
import glob
import re
import sys
from .policycondition import PolicyCondition, check_and_set
from ..matchreference import MatchReferenceFile
from ..verboseprint import dprint

def _empty_list():
    return list()

class MatchFunctionDepend:

    addmodule = re.compile(r'dueca_add_module\s*\(')
    startp = re.compile(r'USEMODULES[ \t]*(#[^\n]*)?\n')
    stopp = re.compile(r'SOURCES|INCLUDEDIRS|DUECA_COMPONENTS|LIBRARIES|COMPILEOPTIONS|COMPILEOPTIONS_PUBLIC|INCLUDEDIRS_PUBLIC|\)')
    pattern = re.compile(r'([a-zA-Z0-9-_]+/)([a-zA-Z0-9-_]+)[ \t]*(#[^\n]*)?')

    def __init__(self, ownproject):
        self.ownproject = ownproject + '/'
        pass

    def __call__(self, text: str, span=None, fpath='CMakeLists.txt'):
        cls = self.__class__

        if span is None:

            taddm = cls.addmodule.search(text)
            if taddm is None:
                dprint(f"No dueca_add_module in {fpath}")
                return None, None
            tstart = cls.startp.search(text)#, taddm.span()[1])
            if tstart is None:
                dprint(f"No USEMODULES module in {fpath}")
                return None, None
            tend = cls.stopp.search(text, tstart.span()[1])
            if tend is None:
                print(
                    "Malformed or mis-read dueca_add_module with USEMODULES"
                    f"in {fpath}", file=sys.stderr)
                return None, None
            istart = tstart.span()[1]
        else:
            istart = span[1]
            tend = cls.stopp.search(text, istart)

        while istart < tend.span()[0]:
            m = cls.pattern.search(text, istart, tend.span()[0])
            if m:
                if m.group(1) == self.ownproject:
                    return m.span(), m
                istart = m.span()[1]
            else:
                break

        return None, None


class HomeDepend(PolicyCondition):
    """Specific check, if a module depends on a sister module in the home project
    """

    # type of result of a holds test
    matchresult = MatchReferenceFile

    # Determine how param arguments need to be stripped
    default_strip = dict(resultvar='both')

    def __init__(self, resultvar='', **kwargs):
        """
        Check for a pattern in the indicated files.

        Parameters
        ----------
        resultvar : Param|str
            Result variable name. Details of the check Will be passed on
            to remaining checks and actions.
        **kwargs : dict
            Remaining, unused variables.

        Returns
        -------
        None.

        """
        self.resultvar = str(resultvar)
        super().__init__(**kwargs)

    def holds(self, p_project, p_path, **kwargs):

        # run and test
        result = []
        newvars = dict()

        matching = glob.glob('*/CMakeLists.txt', recursive=False)

        for fn in matching:
            rf = MatchReferenceFile(MatchFunctionDepend(p_project),
                                    fname=f'{p_path}/{fn}')
            if rf.value:
                result.append(rf)

        check_and_set(self.resultvar, newvars, result)
        res = Fraction(len(result), max(1, len(matching)))
        dprint("HomeDepend", res)
        return (res,
                map(self.__class__.matchresult.explain, result), newvars)

PolicyCondition.register('home-depend', HomeDepend)
