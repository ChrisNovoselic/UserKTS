use e6work
SELECT pl.PTICuserNumber, pl.idTIchnl, ticl.idUSD, usdl.USDUserNumber, sel_sec_15.idVTI as idVTI_SEC_15, vl.idVTI as idVTI_MIN_30, vl.idUnitGrp, dut.TimeIntCoeffDef as timeintcoeffdef, ltl.OutDivCoeff as KoeffI, ltl.OutMultCoeff as KoeffU, ltl.OutAddCoeff as KoeffR, ltl.OutUnit
FROM
(SELECT pl.PTICuserNumber, vl.idVTI
, vl.idclassvti
FROM e6pr.PR_pticlist pl, e6pr.PR_vtilist vl, e6pr.PR_pticoutlist pol, e6pr.PR_dictunitstime dut, e6pr.pr_LinTIMList ltl
WHERE vl.idvti = pol.idvti and pol.idptic = pl.idptic and dut.idunittime = vl.idunittime and ltl.idptic = pl.idptic
and vl.idclassvti = 10
and vl.idclassval = 10
and vl.idunittime = 10
and pl.PTICuserNumber = 469
) sel_sec_15
, e6pr.PR_pticlist pl, e6pr.PR_vtilist vl, e6pr.PR_pticoutlist pol, e6pr.PR_dictunitstime dut, e6pr.pr_LinTIMList ltl

, e6pr.pr_TIchnlList ticl, e6pr.pr_USDList usdl

WHERE vl.idvti = pol.idvti and pol.idptic = pl.idptic and dut.idunittime = vl.idunittime and ltl.idptic = pl.idptic

and pl.idTIchnl = ticl.idTIchnl
and usdl.idUSD = ticl.idUSD 

and vl.idclassvti = sel_sec_15.idclassvti
and vl.idclassval = 15
and vl.idunittime = 20
and pl.PTICuserNumber = sel_sec_15.PTICuserNumber