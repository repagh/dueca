from .policycondition import PolicyCondition, checkAndSet
from ..matchreference import MatchReferenceDco
from ..param import Param

class HomeDco(PolicyCondition):

    # Determine how param arguments need to be stripped
    default_strip = dict(project='both', dco='both', resultvar='both')

    def __init__(self, project: Param, dco: Param, resultvar=None, **kwargs):
        """
        Test whether a dco object is "home".

        Parameters
        ----------
        project : str
            Name or regex of the project supplying the dco object
        dco : str
            Dco object name (without .dco suffix).
        resultvar : str, optional
            Result variable name. Details of the check Will be passed on
            to remaining checks and actions.
        **kwargs : dict
            Remaining, unused variables.

        Returns
        -------
        None.

        """
        self.pproject, self.dco = project, dco
        self.resultvar = resultvar

    def holds(self, p_commobjects, p_project, **kwargs):
        """Check whether a specific module uses a DCO file

        Arguments:
            p_commobjects -- dictionary, keyed with module names, per-module
                             all dco files?
            p_project     -- project hosting the module

        Returns:
            Tuple (bool: true if match found,
            list explaining all maching dcos,
            dict with new variables for further processing
            )
        """

        res = list()
        newvars = dict()

        def isMatch(project, dco):
            if project is not None and dco is not None:
                return self.pproject.match(project) and \
                    project == p_project and self.dco.match(dco)
            if project is None:
                return False
            if dco is not None:
                return project == p_project and self.dco.match(dco)
            return project == p_project

        # each object here is a
        for m, commobj in p_commobjects.items():
            res.append(MatchReferenceDco(isMatch,
                module=m, module_project=p_project,
                fname=f'{p_project}/{m}/comm-objects.lst',
                commobjects=commobj))
            #if commobj.contains(
            #    project=self.pproject, dco=self.dco):
            #    res[-1].value = True
            #    res[-1].commobjects = commobj


        checkAndSet(self.resultvar, newvars, res)

        return (
            len([ r for r in res if r.value ]) > 0,
            (res and
             [ f'Found {self.pproject}/comm-objects/{self.dco}.dco in {r.filename}'
                  for r in res ])
            or [ f'FALSE: no {self.pproject}/comm-objects/{self.dco}.dco found'],
            newvars)

PolicyCondition.register("home-dco", HomeDco)
